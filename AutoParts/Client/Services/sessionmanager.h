#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include "Services/ClientService.h"

class SessionManager : public QObject
{
    Q_OBJECT
public:
    static SessionManager* instance();
    static void destroy();

    void setCurrentUser(const UserData &user);
    UserData currentUser() const;
    bool isLoggedIn() const;
    void logout();

private:
    explicit SessionManager(QObject *parent = nullptr);
    ~SessionManager();
    static SessionManager* m_instance;
    UserData m_currentUser;
    bool m_loggedIn = false;
};

#endif // SESSIONMANAGER_H