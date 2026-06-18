#ifndef USERINFOVIEW_H
#define USERINFOVIEW_H

#include <QString>
#include <QObject>

// Класс для представления vw_users_info (администратор – без паролей)
class UserInfoView : public QObject
{
    Q_OBJECT

public:
    explicit UserInfoView(QObject *parent = nullptr);
    UserInfoView(int userId, const QString &login, const QString &fio,
                 const QString &role);

    int userId() const;
    QString login() const;
    QString fio() const;
    QString role() const;

    static QList<UserInfoView*> loadAll();

private:
    int m_userId;
    QString m_login;
    QString m_fio;
    QString m_role;
};

#endif // USERINFOVIEW_H
