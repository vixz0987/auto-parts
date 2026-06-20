#include "authcontroller.h"
#include "Model/user.h"

User* AuthController::login(const QString &login, const QString &passwordHash)
{
    return User::authenticate(login, passwordHash);
}

bool AuthController::activateUser(const QString &login, const QString &fio,
                                  const QString &passwordHash)
{
    if (passwordHash.isEmpty())
        return false;

    return User::activateUser(login, fio, passwordHash);
}