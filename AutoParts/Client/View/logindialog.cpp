#include "logindialog.h"
#include "ui_logindialog.h"
#include "registerdialog.h"
#include "Utils/sessionmanager.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>

LoginDialog::LoginDialog(TcpClient *client, QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog), m_client(client)
{
    ui->setupUi(this);
    setWindowTitle("Авторизация");

    connect(ui->btnLogin, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(ui->btnRegister, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);
    connect(m_client, &TcpClient::responseReceived, this, &LoginDialog::onLoginResponse);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::onLoginClicked()
{
    QString login = ui->lineLogin->text().trimmed();
    QString password = ui->linePassword->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        return;
    }

    QString cmd = QString("LOGIN %1 %2").arg(login, password);
    m_pendingLoginId = m_client->sendCommand(cmd);
    ui->btnLogin->setEnabled(false);
    ui->btnRegister->setEnabled(false);
}

void LoginDialog::onLoginResponse(quint32 id, const QString &response)
{
    if (id != m_pendingLoginId) return;

    ui->btnLogin->setEnabled(true);
    ui->btnRegister->setEnabled(true);

    if (response.startsWith("OK LOGIN ")) {
        // Извлекаем JSON после "OK LOGIN "
        QString jsonStr = response.mid(QString("OK LOGIN ").length());
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            UserInfo user;
            user.id = obj["id"].toInt();
            user.login = ui->lineLogin->text().trimmed(); // из поля ввода
            user.fio = obj["fio"].toString();
            user.role = obj["role"].toString();
            SessionManager::instance()->setCurrentUser(user);
            QMessageBox::information(this, "Успех", "Вход выполнен как " + user.role + "!");
            accept();
            return;
        }
    }
    // Любая ошибка
    QString errMsg = "Неверный логин или пароль!";
    if (response.startsWith("ERROR ")) {
        errMsg = response.mid(QString("ERROR ").length());
    }
    QMessageBox::critical(this, "Ошибка", errMsg);
}

void LoginDialog::onRegisterClicked()
{
    RegisterDialog regDlg(m_client, this);
    regDlg.exec();
}