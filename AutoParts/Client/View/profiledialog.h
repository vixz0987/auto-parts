#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include "Services/ClientService.h"

namespace Ui { class ProfileDialog; }

class ProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProfileDialog(ClientService* service, int userId, QWidget *parent = nullptr);
    ~ProfileDialog();

private slots:
    void onSave();
    void onOperationSuccess(quint32 requestId, const QString& message);
    void onOperationError(quint32 requestId, const QString& error);

private:
    Ui::ProfileDialog* ui;
    ClientService* m_service;
    int m_userId;
};

#endif // PROFILEDIALOG_H