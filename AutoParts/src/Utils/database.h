#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class Database : public QObject
{
    Q_OBJECT

public:
    static Database* instance();
    static void destroy();

    bool connectToDatabase();
    void disconnect();

    QSqlDatabase getDb() const;

private:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    static Database* m_instance;
    QSqlDatabase db;
};

#endif // DATABASE_H