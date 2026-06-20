#include "sessionmanager.h"

SessionManager* SessionManager::m_instance = nullptr;

SessionManager::SessionManager(QObject *parent) : QObject(parent) {}
SessionManager::~SessionManager() {}

SessionManager* SessionManager::instance()
{
    if (!m_instance) m_instance = new SessionManager();
    return m_instance;
}

void SessionManager::destroy()
{
    delete m_instance;
    m_instance = nullptr;
}

void SessionManager::setCurrentUser(const UserData &user)
{
    m_currentUser = user;
    m_loggedIn = true;
}

UserData SessionManager::currentUser() const
{
    return m_currentUser;
}

bool SessionManager::isLoggedIn() const
{
    return m_loggedIn;
}

void SessionManager::logout()
{
    m_loggedIn = false;
    m_currentUser = UserData();
}