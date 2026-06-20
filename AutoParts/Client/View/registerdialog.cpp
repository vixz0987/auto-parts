#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QMessageBox>
#include <QRegularExpression>

RegisterDialog::RegisterDialog(ClientService* service, QWidget *parent)
    : QDialog(parent), ui(new Ui::RegisterDialog), m_service(service)
{
    ui->setupUi(this);
    setWindowTitle("Активация учётной записи");

    connect(ui->btnRegister, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_service, &ClientService::activationSuccess, this, &RegisterDialog::onActivationSuccess);
    connect(m_service, &ClientService::activationError, this, &RegisterDialog::onActivationError);
}

RegisterDialog::~RegisterDialog() { delete ui; }

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

    ui->btnRegister->setEnabled(false);
    ui->btnCancel->setEnabled(false);
    m_service->activate(login, fio, password);
}

void RegisterDialog::onActivationSuccess()
{
    ui->btnRegister->setEnabled(true);
    ui->btnCancel->setEnabled(true);
    QMessageBox::information(this, "Успех", "Учётная запись активирована! Теперь вы можете войти.");
    accept();
}

void RegisterDialog::onActivationError(const QString& error)
{
    ui->btnRegister->setEnabled(true);
    ui->btnCancel->setEnabled(true);
    QMessageBox::critical(this, "Ошибка", error);
}