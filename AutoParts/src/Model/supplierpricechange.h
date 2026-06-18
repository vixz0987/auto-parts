#include "supplierpricechange.h"
#include "../Utils/database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

SupplierPriceChange::SupplierPriceChange(QObject *parent)
    : QObject(parent), m_supplierId(0), m_priceChangeId(0)
{}

SupplierPriceChange::SupplierPriceChange(int supplierId, int priceChangeId)
    : QObject(nullptr), m_supplierId(supplierId), m_priceChangeId(priceChangeId)
{}

int SupplierPriceChange::supplierId() const { return m_supplierId; }
int SupplierPriceChange::priceChangeId() const { return m_priceChangeId; }

// Создать связь между поставщиком и изменением цены
bool SupplierPriceChange::link(int supplierId, int priceChangeId)
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("INSERT INTO auto_parts.supplier_price_changes "
                  "(supplier_id, price_change_id) VALUES (:sid, :pid)");
    query.bindValue(":sid", supplierId);
    query.bindValue(":pid", priceChangeId);
    if (!query.exec()) {
        qDebug() << "Ошибка создания связи поставщик-изменение цены:"
                 << query.lastError().text();
        return false;
    }
    return true;
}

// Удалить связь
bool SupplierPriceChange::unlink(int supplierId, int priceChangeId)
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("DELETE FROM auto_parts.supplier_price_changes "
                  "WHERE supplier_id = :sid AND price_change_id = :pid");
    query.bindValue(":sid", supplierId);
    query.bindValue(":pid", priceChangeId);
    if (!query.exec()) {
        qDebug() << "Ошибка удаления связи:" << query.lastError().text();
        return false;
    }
    return true;
}

// Проверить существование связи
bool SupplierPriceChange::exists(int supplierId, int priceChangeId)
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM auto_parts.supplier_price_changes "
                  "WHERE supplier_id = :sid AND price_change_id = :pid");
    query.bindValue(":sid", supplierId);
    query.bindValue(":pid", priceChangeId);
    return query.exec() && query.next();
}

// Получить все id изменений цен для заданного поставщика
QList<int> SupplierPriceChange::priceChangeIdsForSupplier(int supplierId)
{
    QList<int> ids;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("SELECT price_change_id FROM auto_parts.supplier_price_changes "
                  "WHERE supplier_id = :sid");
    query.bindValue(":sid", supplierId);
    if (!query.exec()) return ids;
    while (query.next())
        ids.append(query.value(0).toInt());
    return ids;
}
