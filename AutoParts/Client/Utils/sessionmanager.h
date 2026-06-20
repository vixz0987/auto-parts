#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include <QString>

struct UserInfo {
    int id = 0;
    QString login;
    QString fio;
    QString role;
};

class SessionManager : public QObject
{
    Q_OBJECT
public:
    static SessionManager* instance();
    static void destroy();

    void setCurrentUser(const UserInfo &user);
    UserInfo currentUser() const;
    bool isLoggedIn() const;
    void logout();

private:
    explicit SessionManager(QObject *parent = nullptr);
    ~SessionManager();
    static SessionManager* m_instance;
    UserInfo m_currentUser;
    bool m_loggedIn = false;
};

#endif // SESSIONMANAGER_H

