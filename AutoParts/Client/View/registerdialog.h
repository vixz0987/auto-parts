#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "TcpClient/tcpclient.h"

namespace Ui { class RegisterDialog; }

class RegisterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterDialog(TcpClient *client, QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void onRegisterClicked();
    void onActivateResponse(quint32 id, const QString &response);

private:
    Ui::RegisterDialog *ui;
    TcpClient *m_client;
    quint32 m_pendingId = 0;
};

#endif // REGISTERDIALOG_H
