#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "TcpClient/tcpclient.h"

namespace Ui { class LoginDialog; }

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(TcpClient *client, QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onLoginResponse(quint32 id, const QString &response);

private:
    Ui::LoginDialog *ui;
    TcpClient *m_client;
    quint32 m_pendingLoginId = 0;
};

#endif // LOGINDIALOG_H

