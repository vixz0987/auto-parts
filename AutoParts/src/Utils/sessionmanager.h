#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include "../Model/user.h"

class SessionManager : public QObject
{
    Q_OBJECT

public:
    static SessionManager* instance();
    static void destroy();

    User* currentUser() const;
    void setCurrentUser(User *user);
    bool isLoggedIn() const;

private:
    explicit SessionManager(QObject *parent = nullptr);
    ~SessionManager();

    static SessionManager* m_instance;
    User *m_currentUser;
};

#endif // SESSIONMANAGER_H