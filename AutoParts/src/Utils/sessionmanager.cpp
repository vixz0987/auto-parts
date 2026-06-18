#include "sessionmanager.h"

SessionManager* SessionManager::m_instance = nullptr;

SessionManager::SessionManager(QObject *parent)
    : QObject(parent), m_currentUser(nullptr) {}

SessionManager::~SessionManager()
{
    delete m_currentUser;
}

SessionManager* SessionManager::instance()
{
    if (!m_instance)
        m_instance = new SessionManager();
    return m_instance;
}

void SessionManager::destroy()
{
    delete m_instance;
    m_instance = nullptr;
}

User* SessionManager::currentUser() const
{
    return m_currentUser;
}

void SessionManager::setCurrentUser(User *user)
{
    delete m_currentUser;
    m_currentUser = user;
}

bool SessionManager::isLoggedIn() const
{
    return m_currentUser != nullptr;
}