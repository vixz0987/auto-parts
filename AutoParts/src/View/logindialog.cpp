#include "logindialog.h"
#include "ui_logindialog.h"
#include "registerdialog.h"
#include "../Controller/authcontroller.h"
#include "../Utils/sessionmanager.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowTitle("Авторизация — Автозапчасти");

    connect(ui->btnLogin, &QPushButton::clicked,
            this, &LoginDialog::onLoginClicked);
    connect(ui->btnRegister, &QPushButton::clicked,
            this, &LoginDialog::onRegisterClicked);
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

    User *user = AuthController::login(login, password);
    if (user) {
        SessionManager::instance()->setCurrentUser(user);
        QMessageBox::information(this, "Успех",
                                 "Вход выполнен как " + user->role() + "!");
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка",
                              "Неверный логин или пароль!");
    }
}

void LoginDialog::onRegisterClicked()
{
    RegisterDialog regDlg(this);
    regDlg.exec();
}