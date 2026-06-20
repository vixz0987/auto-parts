#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include "TcpClient/tcpclient.h"

namespace Ui { class ProfileDialog; }

class ProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProfileDialog(TcpClient *client, int userId, QWidget *parent = nullptr);
    ~ProfileDialog();

private slots:
    void onSave();
    void onServerResponse(quint32 id, const QString &response);

private:
    Ui::ProfileDialog *ui;
    TcpClient *m_client;
    int m_userId;
    quint32 m_pendingId = 0;
};

#endif // PROFILEDIALOG_H