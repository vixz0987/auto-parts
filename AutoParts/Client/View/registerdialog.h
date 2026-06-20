#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "Services/ClientService.h"

namespace Ui { class RegisterDialog; }

class RegisterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterDialog(ClientService* service, QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void onRegisterClicked();
    void onActivationSuccess();
    void onActivationError(const QString& error);

private:
    Ui::RegisterDialog* ui;
    ClientService* m_service;
};

#endif // REGISTERDIALOG_H