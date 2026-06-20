#include "database.h"

Database* Database::m_instance = nullptr;

Database::Database(QObject *parent) : QObject(parent) {}

Database::~Database() { disconnect(); }

Database* Database::instance()
{
    if (!m_instance) m_instance = new Database();
    return m_instance;
}

void Database::destroy()
{
    delete m_instance;
    m_instance = nullptr;
}

bool Database::connectToDatabase()
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("auto_parts_company");
    db.setUserName("postgres");
    db.setPassword("1postgre1");

    if (!db.open()) {
        qCritical() << "Failed to connect to PostgreSQL:" << db.lastError().text();
        return false;
    }
    qInfo() << "Connected to PostgreSQL";
    return true;
}

void Database::disconnect()
{
    if (db.isOpen()) db.close();
}

QSqlDatabase Database::getDb() const { return db; }