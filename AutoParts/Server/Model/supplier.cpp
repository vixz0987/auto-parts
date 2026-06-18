#include "supplier.h"
#include "Utils/database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Supplier::Supplier(QObject *parent)
    : QObject(parent), m_supplierId(0)
{}

Supplier::Supplier(int supplierId, const QString &name,
                   const QString &phone, const QString &address)
    : QObject(nullptr), m_supplierId(supplierId), m_name(name),
    m_phone(phone), m_address(address)
{}

int Supplier::supplierId() const { return m_supplierId; }
QString Supplier::name() const { return m_name; }
QString Supplier::phone() const { return m_phone; }
QString Supplier::address() const { return m_address; }

void Supplier::setName(const QString &name) { m_name = name; }
void Supplier::setPhone(const QString &phone) { m_phone = phone; }
void Supplier::setAddress(const QString &address) { m_address = address; }

// Сохранение нового поставщика в БД
bool Supplier::save()
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("INSERT INTO auto_parts.suppliers (name, phone, address) "
                  "VALUES (:name, :phone, :address) RETURNING supplier_id");
    query.bindValue(":name", m_name);
    query.bindValue(":phone", m_phone);
    query.bindValue(":address", m_address);

    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка сохранения поставщика:" << query.lastError().text();
        return false;
    }
    m_supplierId = query.value(0).toInt();
    return true;
}

// Обновление существующего поставщика
bool Supplier::update()
{
    if (m_supplierId == 0) return false;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("UPDATE auto_parts.suppliers SET name = :name, "
                  "phone = :phone, address = :address WHERE supplier_id = :id");
    query.bindValue(":name", m_name);
    query.bindValue(":phone", m_phone);
    query.bindValue(":address", m_address);
    query.bindValue(":id", m_supplierId);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления поставщика:" << query.lastError().text();
        return false;
    }
    return true;
}

// Удаление поставщика
bool Supplier::remove()
{
    if (m_supplierId == 0) return false;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("DELETE FROM auto_parts.suppliers WHERE supplier_id = :id");
    query.bindValue(":id", m_supplierId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления поставщика:" << query.lastError().text();
        return false;
    }
    return true;
}

// Загрузка одного поставщика по идентификатору
Supplier* Supplier::loadById(int supplierId)
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("SELECT supplier_id, name, phone, address "
                  "FROM auto_parts.suppliers WHERE supplier_id = :id");
    query.bindValue(":id", supplierId);
    if (!query.exec() || !query.next())
        return nullptr;
    return new Supplier(query.value("supplier_id").toInt(),
                        query.value("name").toString(),
                        query.value("phone").toString(),
                        query.value("address").toString());
}

// Загрузка всех поставщиков
QList<Supplier*> Supplier::loadAll()
{
    QList<Supplier*> suppliers;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.exec("SELECT supplier_id, name, phone, address "
               "FROM auto_parts.suppliers ORDER BY supplier_id");
    while (query.next()) {
        suppliers.append(new Supplier(query.value("supplier_id").toInt(),
                                      query.value("name").toString(),
                                      query.value("phone").toString(),
                                      query.value("address").toString()));
    }
    return suppliers;
}
