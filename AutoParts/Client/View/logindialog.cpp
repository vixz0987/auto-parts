#include "logindialog.h"
#include "ui_logindialog.h"
#include "registerdialog.h"
#include "Services/sessionmanager.h"
#include <QMessageBox>

LoginDialog::LoginDialog(ClientService* service, QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog), m_service(service)
{
    ui->setupUi(this);
    setWindowTitle("Авторизация");

    connect(ui->btnLogin, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(ui->btnRegister, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);
    connect(m_service, &ClientService::loginSuccess, this, &LoginDialog::onLoginSuccess);
    connect(m_service, &ClientService::loginError, this, &LoginDialog::onLoginError);
}

LoginDialog::~LoginDialog() { delete ui; }

void LoginDialog::onLoginClicked()
{
    QString login = ui->lineLogin->text().trimmed();
    QString password = ui->linePassword->text().trimmed();
    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        return;
    }
    ui->btnLogin->setEnabled(false);
    ui->btnRegister->setEnabled(false);
    m_service->login(login, password);
}

void LoginDialog::onLoginSuccess(const UserData& user)
{
    ui->btnLogin->setEnabled(true);
    ui->btnRegister->setEnabled(true);
    SessionManager::instance()->setCurrentUser(user);
    QMessageBox::information(this, "Успех", "Вход выполнен как " + user.role + "!");
    accept();
}

void LoginDialog::onLoginError(const QString& error)
{
    ui->btnLogin->setEnabled(true);
    ui->btnRegister->setEnabled(true);
    QMessageBox::critical(this, "Ошибка", error);
}

void LoginDialog::onRegisterClicked()
{
    RegisterDialog regDlg(m_service, this);
    regDlg.exec();
}