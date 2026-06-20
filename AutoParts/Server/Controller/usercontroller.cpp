#include "usercontroller.h"
#include "Model/user.h"
#include "Model/userinfoview.h"

// Создание пользователя администратором (пароль остаётся NULL)
bool UserController::createUser(const QString &login, const QString &fio,
                                const QString &role)
{
    return User::createUser(login, fio, role);
}

// Изменение роли
bool UserController::changeRole(int userId, const QString &newRole)
{
    User *user = User::loadById(userId);
    if (!user) return false;
    user->setRole(newRole);
    bool result = user->update();
    delete user;
    return result;
}

// Удаление пользователя
bool UserController::deleteUser(int userId)
{
    User *user = User::loadById(userId);
    if (!user) return false;
    bool result = user->remove();
    delete user;
    return result;
}

// Получение списка пользователей (без паролей)
QList<UserInfoView*> UserController::getAllUsers()
{
    return UserInfoView::loadAll();
}

// Изменение данных
bool UserController::changePassword(int userId, const QString &oldPassword, const QString &newPassword)
{
    return User::changePassword(userId, oldPassword, newPassword);
}

bool UserController::changeFio(int userId, const QString &newFio)
{
    return User::changeFio(userId, newFio);
}