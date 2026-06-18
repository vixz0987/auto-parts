#ifndef USER_H
#define USER_H

#include <QString>
#include <QObject>

// класс, описывающий пользователя системы
// содержит поля, соответствующие таблице auto_parts.users
// и методы для аутентификации, регистрации и управления профилем.
class User : public QObject
{
    Q_OBJECT

public:
    explicit User(QObject *parent = nullptr);
    User(int userId, const QString &login, const QString &fio,
         const QString &passwordHash, const QString &role);

    // геттеры
    int userId() const;
    QString login() const;
    QString fio() const;
    QString passwordHash() const; // хэш с солью (PBKDF2)
    QString role() const; // admin, manager, accountant

    // сеттеры
    void setFio(const QString &fio);
    void setPasswordHash(const QString &hash);
    void setRole(const QString &role);

    // статические методы для работы с таблицей users

    // аутентификация: возвращает пользователя, если пароль верен и аккаунт активирован
    static User* authenticate(const QString &login, const QString &password);

    // активация учётной записи: пользователь задаёт пароль, если запись существует,
    // ФИО совпадает и пароль ещё не был задан.
    static bool activateUser(const QString &login, const QString &fio,
                             const QString &password);

    // создание нового пользователя администратором (пароль не задаётся)
    static bool createUser(const QString &login, const QString &fio,
                           const QString &role);

    bool save(); // вставка новой записи
    bool update(); // обновление текущей
    bool remove(); // удаление записи из БД
    static User* loadById(int userId);
    static QList<User*> loadAll();

    // проверка сложности пароля через регулярное выражение
    static bool isPasswordValid(const QString &password);

private:
    int m_userId;
    QString m_login;
    QString m_fio;
    QString m_passwordHash;
    QString m_role;
};

#endif // USER_H