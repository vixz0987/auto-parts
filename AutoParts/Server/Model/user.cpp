#include "user.h"
#include "Utils/database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QRegularExpression>
#include <QMessageAuthenticationCode>
#include <QRandomGenerator>
#include <QtGlobal>
#include <cstring>

// Реализация класса User

User::User(QObject *parent)
    : QObject(parent), m_userId(0)
{}

User::User(int userId, const QString &login, const QString &fio,
           const QString &passwordHash, const QString &role)
    : QObject(nullptr), m_userId(userId), m_login(login),
    m_fio(fio), m_passwordHash(passwordHash), m_role(role)
{}

int User::userId() const { return m_userId; }
QString User::login() const { return m_login; }
QString User::fio() const { return m_fio; }
QString User::passwordHash() const { return m_passwordHash; }
QString User::role() const { return m_role; }

void User::setFio(const QString &fio) { m_fio = fio; }
void User::setPasswordHash(const QString &hash) { m_passwordHash = hash; }
void User::setRole(const QString &role) { m_role = role; }

// аутентификация: пароль должен быть задан (не NULL и не пустая строка)
User* User::authenticate(const QString &login, const QString &passwordHash)
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("SELECT user_id, login, fio, password_hash, role "
                  "FROM auto_parts.users WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec() || !query.next()) {
        qDebug() << "Пользователь не найден:" << login;
        return nullptr;
    }

    QString storedHash = query.value("password_hash").toString();
    if (storedHash.isEmpty() || storedHash.isNull()) {
        qDebug() << "Учётная запись не активирована:" << login;
        return nullptr;
    }

    if (passwordHash != storedHash) {
        qDebug() << "Неверный пароль для:" << login;
        return nullptr;
    }

    return new User(query.value("user_id").toInt(),
                    query.value("login").toString(),
                    query.value("fio").toString(),
                    storedHash,
                    query.value("role").toString());
}

// активация учётной записи: проверяем логин, ФИО и что пароль не задан
bool User::activateUser(const QString &login, const QString &fio, const QString &passwordHash)
{
    if (passwordHash.isEmpty()) return false;

    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);

    query.prepare("SELECT user_id, password_hash FROM auto_parts.users "
                  "WHERE login = :login AND fio = :fio");
    query.bindValue(":login", login);
    query.bindValue(":fio", fio);

    if (!query.exec() || !query.next()) {
        qDebug() << "Не найдена запись для активации";
        return false;
    }

    QString currentHash = query.value("password_hash").toString();
    if (!currentHash.isEmpty() && !currentHash.isNull()) {
        qDebug() << "Пароль уже задан для:" << login;
        return false;
    }

    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE auto_parts.users SET password_hash = :hash "
                        "WHERE login = :login");
    updateQuery.bindValue(":hash", passwordHash);
    updateQuery.bindValue(":login", login);

    if (!updateQuery.exec()) {
        qDebug() << "Ошибка установки пароля:" << updateQuery.lastError().text();
        return false;
    }
    return true;
}

// создание пользователя администратором (без пароля)
bool User::createUser(const QString &login, const QString &fio,
                      const QString &role)
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("INSERT INTO auto_parts.users (login, fio, role) "
                  "VALUES (:login, :fio, :role)");
    query.bindValue(":login", login);
    query.bindValue(":fio", fio);
    query.bindValue(":role", role);

    if (!query.exec()) {
        qDebug() << "Ошибка создания пользователя:" << query.lastError().text();
        return false;
    }
    return true;
}


// сохранение нового объекта User в БД (возвращает id)
bool User::save()
{
    if (m_userId != 0) return false; // уже существует в БД
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("INSERT INTO auto_parts.users (login, fio, password_hash, role) "
                  "VALUES (:login, :fio, :hash, :role) "
                  "RETURNING user_id");
    query.bindValue(":login", m_login);
    query.bindValue(":fio", m_fio);
    query.bindValue(":hash", m_passwordHash);
    query.bindValue(":role", m_role);

    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка сохранения пользователя:" << query.lastError().text();
        return false;
    }
    m_userId = query.value(0).toInt();
    return true;
}

// обновление существующей записи в БД
bool User::update()
{
    if (m_userId == 0) return false;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("UPDATE auto_parts.users SET fio = :fio, "
                  "password_hash = :hash, role = :role "
                  "WHERE user_id = :id");
    query.bindValue(":fio", m_fio);
    query.bindValue(":hash", m_passwordHash);
    query.bindValue(":role", m_role);
    query.bindValue(":id", m_userId);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления пользователя:" << query.lastError().text();
        return false;
    }
    return true;
}

// удаление записи из БД
bool User::remove()
{
    if (m_userId == 0) return false;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("DELETE FROM auto_parts.users WHERE user_id = :id");
    query.bindValue(":id", m_userId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления пользователя:" << query.lastError().text();
        return false;
    }
    return true;
}

// загрузка одного пользователя по идентификатору
User* User::loadById(int userId)
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("SELECT user_id, login, fio, password_hash, role "
                  "FROM auto_parts.users WHERE user_id = :id");
    query.bindValue(":id", userId);
    if (!query.exec() || !query.next())
        return nullptr;
    return new User(query.value("user_id").toInt(),
                    query.value("login").toString(),
                    query.value("fio").toString(),
                    query.value("password_hash").toString(),
                    query.value("role").toString());
}

// загрузка всех пользователей из БД
QList<User*> User::loadAll()
{
    QList<User*> users;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.exec("SELECT user_id, login, fio, password_hash, role "
               "FROM auto_parts.users ORDER BY user_id");
    while (query.next()) {
        users.append(new User(query.value("user_id").toInt(),
                              query.value("login").toString(),
                              query.value("fio").toString(),
                              query.value("password_hash").toString(),
                              query.value("role").toString()));
    }
    return users;
}

bool User::changePassword(int userId, const QString &oldPasswordHash, const QString &newPasswordHash)
{
    if (newPasswordHash.isEmpty()) return false;

    User *user = loadById(userId);
    if (!user) return false;

    // Сравниваем хеши старых паролей
    QString storedHash = user->passwordHash();
    if (storedHash.isEmpty() || oldPasswordHash != storedHash) {
        delete user;
        return false;
    }

    // Сохраняем новый хеш
    user->setPasswordHash(newPasswordHash);
    bool ok = user->update();
    delete user;
    return ok;
}

bool User::changeFio(int userId, const QString &newFio)
{
    if (newFio.trimmed().isEmpty()) return false;

    User *user = loadById(userId);
    if (!user) return false;

    user->setFio(newFio.trimmed());
    bool ok = user->update();
    delete user;
    return ok;
}