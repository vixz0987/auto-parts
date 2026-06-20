#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "Services/clientservice.h"

namespace Ui { class LoginDialog; }

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(ClientService* service, QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onLoginSuccess(const UserData& user);
    void onLoginError(const QString& error);

private:
    Ui::LoginDialog* ui;
    ClientService* m_service;
};

#endif // LOGINDIALOG_H