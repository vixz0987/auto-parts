#include "authcontroller.h"
#include "Model/user.h"

User* AuthController::login(const QString &login, const QString &password)
{
    return User::authenticate(login, password);
}

bool AuthController::activateUser(const QString &login, const QString &fio,
                                  const QString &password)
{
    if (!User::isPasswordValid(password))
        return false;
    return User::activateUser(login, fio, password);
}