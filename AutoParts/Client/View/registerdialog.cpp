#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>

RegisterDialog::RegisterDialog(TcpClient *client, QWidget *parent)
    : QDialog(parent), ui(new Ui::RegisterDialog), m_client(client)
{
    ui->setupUi(this);
    setWindowTitle("Активация учётной записи");

    connect(ui->btnRegister, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_client, &TcpClient::responseReceived, this, &RegisterDialog::onActivateResponse);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::onRegisterClicked()
{
    QString login = ui->lineLogin->text().trimmed();
    QString fio = ui->lineFio->text().trimmed();
    QString password = ui->linePassword->text();
    QString confirm = ui->lineConfirmPassword->text();

    if (login.isEmpty() || fio.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Все поля обязательны для заполнения!");
        return;
    }
    if (password != confirm) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают!");
        return;
    }
    QRegularExpression re("^(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*()_+=-]).{8,}$");
    if (!re.match(password).hasMatch()) {
        QMessageBox::warning(this, "Слабый пароль",
                             "Пароль должен быть не менее 8 символов, содержать заглавную букву, цифру и спецсимвол.");
        return;
    }

    QJsonObject obj;
    obj["login"] = login;
    obj["fio"] = fio;
    obj["password"] = password;
    QJsonDocument doc(obj);
    QString cmd = "ACTIVATE " + doc.toJson(QJsonDocument::Compact);
    m_pendingId = m_client->sendCommand(cmd);

    ui->btnRegister->setEnabled(false);
    ui->btnCancel->setEnabled(false);
}

void RegisterDialog::onActivateResponse(quint32 id, const QString &response)
{
    if (id != m_pendingId) return;

    ui->btnRegister->setEnabled(true);
    ui->btnCancel->setEnabled(true);

    if (response.startsWith("OK ACTIVATED")) {
        QMessageBox::information(this, "Успех", "Учётная запись активирована! Теперь вы можете войти.");
        accept();
    } else {
        QString err = "Не удалось активировать учётную запись.";
        if (response.startsWith("ERROR ")) {
            err = response.mid(QString("ERROR ").length());
        }
        QMessageBox::critical(this, "Ошибка", err);
    }
}
