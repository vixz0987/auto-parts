#include "supply.h"
#include "Utils/database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Supply::Supply(QObject *parent)
    : QObject(parent), m_supplyId(0), m_quantity(0),
    m_supplierId(0), m_priceChangeId(0)
{}

Supply::Supply(int supplyId, const QDate &supplyDate, int quantity,
               int supplierId, int priceChangeId)
    : QObject(nullptr), m_supplyId(supplyId), m_supplyDate(supplyDate),
    m_quantity(quantity), m_supplierId(supplierId), m_priceChangeId(priceChangeId)
{}

int Supply::supplyId() const { return m_supplyId; }
QDate Supply::supplyDate() const { return m_supplyDate; }
int Supply::quantity() const { return m_quantity; }
int Supply::supplierId() const { return m_supplierId; }
int Supply::priceChangeId() const { return m_priceChangeId; }

void Supply::setSupplyDate(const QDate &supplyDate) { m_supplyDate = supplyDate; }
void Supply::setQuantity(int quantity) { m_quantity = quantity; }
void Supply::setSupplierId(int supplierId) { m_supplierId = supplierId; }
void Supply::setPriceChangeId(int priceChangeId) { m_priceChangeId = priceChangeId; }

// Сохранение новой поставки
bool Supply::save()
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("INSERT INTO auto_parts.supplies "
                  "(supply_date, quantity, supplier_id, price_change_id) "
                  "VALUES (:supply_date, :quantity, :supplier_id, :price_change_id) "
                  "RETURNING supply_id");
    query.bindValue(":supply_date", m_supplyDate);
    query.bindValue(":quantity", m_quantity);
    query.bindValue(":supplier_id", m_supplierId);
    query.bindValue(":price_change_id", m_priceChangeId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка сохранения поставки:" << query.lastError().text();
        return false;
    }
    m_supplyId = query.value(0).toInt();
    return true;
}

// Обновление поставки
bool Supply::update()
{
    if (m_supplyId == 0) return false;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("UPDATE auto_parts.supplies SET supply_date = :supply_date, "
                  "quantity = :quantity, supplier_id = :supplier_id, "
                  "price_change_id = :price_change_id WHERE supply_id = :id");
    query.bindValue(":supply_date", m_supplyDate);
    query.bindValue(":quantity", m_quantity);
    query.bindValue(":supplier_id", m_supplierId);
    query.bindValue(":price_change_id", m_priceChangeId);
    query.bindValue(":id", m_supplyId);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления поставки:" << query.lastError().text();
        return false;
    }
    return true;
}

// Удаление поставки
bool Supply::remove()
{
    if (m_supplyId == 0) return false;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("DELETE FROM auto_parts.supplies WHERE supply_id = :id");
    query.bindValue(":id", m_supplyId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления поставки:" << query.lastError().text();
        return false;
    }
    return true;
}

// Загрузка поставки по идентификатору
Supply* Supply::loadById(int supplyId)
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("SELECT supply_id, supply_date, quantity, supplier_id, price_change_id "
                  "FROM auto_parts.supplies WHERE supply_id = :id");
    query.bindValue(":id", supplyId);
    if (!query.exec() || !query.next())
        return nullptr;
    return new Supply(query.value("supply_id").toInt(),
                      query.value("supply_date").toDate(),
                      query.value("quantity").toInt(),
                      query.value("supplier_id").toInt(),
                      query.value("price_change_id").toInt());
}

// Загрузка всех поставок
QList<Supply*> Supply::loadAll()
{
    QList<Supply*> list;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.exec("SELECT supply_id, supply_date, quantity, supplier_id, price_change_id "
               "FROM auto_parts.supplies ORDER BY supply_date DESC");
    while (query.next()) {
        list.append(new Supply(query.value("supply_id").toInt(),
                               query.value("supply_date").toDate(),
                               query.value("quantity").toInt(),
                               query.value("supplier_id").toInt(),
                               query.value("price_change_id").toInt()));
    }
    return list;
}

// Загрузка поставок конкретного поставщика
QList<Supply*> Supply::loadBySupplier(int supplierId)
{
    QList<Supply*> list;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("SELECT supply_id, supply_date, quantity, supplier_id, price_change_id "
                  "FROM auto_parts.supplies WHERE supplier_id = :sid "
                  "ORDER BY supply_date DESC");
    query.bindValue(":sid", supplierId);
    if (!query.exec()) return list;
    while (query.next()) {
        list.append(new Supply(query.value("supply_id").toInt(),
                               query.value("supply_date").toDate(),
                               query.value("quantity").toInt(),
                               query.value("supplier_id").toInt(),
                               query.value("price_change_id").toInt()));
    }
    return list;
}