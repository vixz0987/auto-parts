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
    QString passwordHash() const;
    QString role() const;

    // сеттеры
    void setFio(const QString &fio);
    void setPasswordHash(const QString &hash);
    void setRole(const QString &role);

    // статические методы для работы с таблицей users
    static User* authenticate(const QString &login, const QString &passwordHash);
    static bool activateUser(const QString &login, const QString &fio, const QString &passwordHash);
    static bool createUser(const QString &login, const QString &fio,
                           const QString &role);
    static bool changePassword(int userId, const QString &oldPasswordHash, const QString &newPasswordHash);
    static bool changeFio(int userId, const QString &newFio);

    bool save(); // вставка новой записи
    bool update(); // обновление текущей
    bool remove(); // удаление записи из БД
    static User* loadById(int userId);
    static QList<User*> loadAll();


private:
    int m_userId;
    QString m_login;
    QString m_fio;
    QString m_passwordHash;
    QString m_role;
};

#endif // USER_H