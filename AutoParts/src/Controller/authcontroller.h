#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include <QString>

class User;

// Контроллер для операций входа и регистрации (активации)
class AuthController
{
public:
    // Попытка входа - возвращает объект User при успехе, иначе nullptr
    static User* login(const QString &login, const QString &password);

    // Активация учётной записи: логин, ФИО, новый пароль
    static bool activateUser(const QString &login, const QString &fio,
                             const QString &password);
};

#endif // AUTHCONTROLLER_H
