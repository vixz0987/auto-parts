#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include <QString>
#include <QList>

class User;
class UserInfoView;

// Контроллер для административных операций с пользователями
class UserController
{
public:
    static bool createUser(const QString &login, const QString &fio, const QString &role);
    static bool changeRole(int userId, const QString &newRole);
    static bool deleteUser(int userId);
    static QList<UserInfoView*> getAllUsers();
    static bool changePassword(int userId, const QString &oldPasswordHash, const QString &newPasswordHash);
    static bool changeFio(int userId, const QString &newFio);
};

#endif // USERCONTROLLER_H