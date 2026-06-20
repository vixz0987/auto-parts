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
    // Создание пользователя администратором (без пароля)
    static bool createUser(const QString &login, const QString &fio,
                           const QString &role);

    // Изменение роли пользователя (повышение/понижение)
    static bool changeRole(int userId, const QString &newRole);

    // Удаление пользователя
    static bool deleteUser(int userId);

    // Получение списка всех пользователей (read-only, без паролей)
    static QList<UserInfoView*> getAllUsers();

    // Смена пароля и личных данных
    static bool changePassword(int userId, const QString &oldPassword, const QString &newPassword);
    static bool changeFio(int userId, const QString &newFio);
};

#endif // USERCONTROLLER_H