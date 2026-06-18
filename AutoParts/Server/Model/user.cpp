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

// Вспомогательные функции для хэширования паролей (PBKDF2-HMAC-SHA256)

// параметры PBKDF2: 10000 итераций, соль 16 байт, ключ 32 байта
static const int PBKDF2_ITERATIONS = 10000;
static const int SALT_SIZE          = 16;
static const int DERIVED_KEY_LEN    = 32;

// Генерирует случайную соль в виде hex-строки
static QString generateSalt()
{
    QByteArray saltBytes(SALT_SIZE, '\0');
    QRandomGenerator::system()->fillRange(
        reinterpret_cast<quint32*>(saltBytes.data()),
        SALT_SIZE / sizeof(quint32));
    return QString::fromLatin1(saltBytes.toHex());
}

// Реализация PBKDF2-HMAC-SHA256 согласно RFC 2898
static QByteArray pbkdf2_hmac_sha256(const QByteArray &password,
                                     const QByteArray &salt,
                                     int iterations,
                                     int dkLen)
{
    const int hmacLen = 32;   // SHA-256 даёт 32 байта
    QByteArray derivedKey;
    derivedKey.resize(dkLen);

    int blocks = (dkLen + hmacLen - 1) / hmacLen;
    QByteArray U, currentBlock;

    for (int i = 1; i <= blocks; ++i) {
        // Формируем входной блок: соль+номер блока
        QByteArray input = salt;
        input.append(static_cast<char>((i >> 24) & 0xFF));
        input.append(static_cast<char>((i >> 16) & 0xFF));
        input.append(static_cast<char>((i >> 8) & 0xFF));
        input.append(static_cast<char>(i & 0xFF));

        // Первая итерация HMAC
        QMessageAuthenticationCode hmac(QCryptographicHash::Sha256);
        hmac.setKey(password);
        hmac.addData(input);
        U = hmac.result();
        currentBlock = U;

        // Последующие итерации и XOR
        for (int j = 1; j < iterations; ++j) {
            hmac.reset();
            hmac.setKey(password);
            hmac.addData(U);
            U = hmac.result();
            for (int k = 0; k < U.size(); ++k)
                currentBlock[k] = currentBlock[k] ^ U[k];
        }

        // Копируем полученный блок в итоговый ключ
        int bytesToCopy = qMin(hmacLen, dkLen - (i - 1) * hmacLen);
        memcpy(derivedKey.data() + (i - 1) * hmacLen,
               currentBlock.constData(), bytesToCopy);
    }
    return derivedKey;
}

// Хэширует пароль с заданной солью
static QString hashPassword(const QString &password, const QString &salt)
{
    QByteArray derivedKey = pbkdf2_hmac_sha256(
        password.toUtf8(),
        QByteArray::fromHex(salt.toUtf8()),
        PBKDF2_ITERATIONS,
        DERIVED_KEY_LEN);
    return QString::fromLatin1(derivedKey.toHex());
}

// Упаковывает соль и хэш в одну строку для хранения в БД
static QString packHash(const QString &salt, const QString &hash)
{
    return salt + ":" + hash;
}

// Проверяет пароль: извлекает соль из хранимой строки, хэширует введённый пароль
static bool verifyPassword(const QString &password, const QString &storedHash)
{
    int colon = storedHash.indexOf(':');
    if (colon < 0) return false;
    QString salt = storedHash.left(colon);
    QString hash = storedHash.mid(colon + 1);
    return hashPassword(password, salt) == hash;
}

// Полный цикл: генерирует соль, хэширует пароль, возвращает строку для БД
static QString generatePasswordHash(const QString &password)
{
    QString salt = generateSalt();
    QString hash = hashPassword(password, salt);
    return packHash(salt, hash);
}

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
User* User::authenticate(const QString &login, const QString &password)
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
    // проверяем, что пароль установлен (не пустая строка и не NULL)
    if (storedHash.isEmpty() || storedHash.isNull()) {
        qDebug() << "Учётная запись не активирована:" << login;
        return nullptr;
    }
    if (!verifyPassword(password, storedHash)) {
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
bool User::activateUser(const QString &login, const QString &fio,
                        const QString &password)
{
    if (!isPasswordValid(password))
        return false;

    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);

    // ищем запись с таким логином и ФИО, у которой пароль ещё пуст
    query.prepare("SELECT user_id, password_hash FROM auto_parts.users "
                  "WHERE login = :login AND fio = :fio");
    query.bindValue(":login", login);
    query.bindValue(":fio", fio);

    if (!query.exec() || !query.next()) {
        qDebug() << "Не найдена запись для активации с такими логином/ФИО";
        return false;
    }

    QString currentHash = query.value("password_hash").toString();
    if (!currentHash.isEmpty() && !currentHash.isNull()) {
        qDebug() << "Пароль уже задан для:" << login;
        return false;
    }

    // генерируем хэш и обновляем запись
    QString newHash = generatePasswordHash(password);
    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE auto_parts.users SET password_hash = :hash "
                        "WHERE login = :login");
    updateQuery.bindValue(":hash", newHash);
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

// проверка пароля регулярным выражением
bool User::isPasswordValid(const QString &password)
{
    QRegularExpression re("^(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*()_+=-]).{8,}$");
    return re.match(password).hasMatch();
}