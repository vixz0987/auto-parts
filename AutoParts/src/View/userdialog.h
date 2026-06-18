#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>

namespace Ui {
class UserDialog;
}

// Диалог для создания/редактирования пользователя администратором
class UserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDialog(QWidget *parent = nullptr);
    ~UserDialog();

    QString login() const;
    QString fio() const;
    QString role() const;

    void setLogin(const QString &login);
    void setFio(const QString &fio);
    void setRole(const QString &role);
    void setLoginReadOnly(bool readOnly);   // для редактирования логин не меняется

private slots:
    void onAccept();

private:
    Ui::UserDialog *ui;
};

#endif // USERDIALOG_H
