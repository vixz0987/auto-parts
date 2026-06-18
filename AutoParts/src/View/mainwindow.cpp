#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "../Utils/sessionmanager.h"
#include <QApplication>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LoginDialog loginDlg(this);
    if (loginDlg.exec() != QDialog::Accepted) {
        QApplication::quit();
        return;
    }

    User *user = SessionManager::instance()->currentUser();
    if (user) {
        ui->statusbar->showMessage(
            QString("Авторизован: %1 (%2)").arg(user->fio(), user->role()));
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось получить данные пользователя.");
        QApplication::quit();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}