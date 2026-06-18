#include "database.h"
#include <QMessageBox>

Database* Database::m_instance = nullptr;

Database::Database(QObject *parent) : QObject(parent)
{
    // не трогаем БД, только создаём объект
}

Database::~Database()
{
    disconnect();
}

Database* Database::instance()
{
    if (!m_instance) {
        m_instance = new Database();
    }
    return m_instance;
}

void Database::destroy()
{
    delete m_instance;
    m_instance = nullptr;
}

bool Database::connectToDatabase()
{
    // драйвер QPSQL
    db = QSqlDatabase::addDatabase("QPSQL");

    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("auto_parts_company");
    db.setUserName("postgres");
    db.setPassword("1postgre1");

    if (!db.open()) {
        QString error = db.lastError().text();
        qDebug() << "ОШИБКА подключения к PostgreSQL:" << error;
        QMessageBox::critical(nullptr, "Ошибка подключения", error);
        return false;
    }

    qDebug() << "Успешное подключение к PostgreSQL!";
    QMessageBox::information(nullptr, "Успех", "Подключение к PostgreSQL установлено!");
    return true;
}

void Database::disconnect()
{
    if (db.isOpen()) {
        db.close();
    }
}

QSqlDatabase Database::getDb() const
{
    return db;
}