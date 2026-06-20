#include "pricechange.h"
#include "Utils/database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

PriceChange::PriceChange(QObject *parent)
    : QObject(parent), m_priceChangeId(0), m_detailId(0), m_price(0.0), m_supplierId(0)
{}

PriceChange::PriceChange(int priceChangeId, int detailId, const QDate &changeDate,
                         double price, int supplierId)
    : QObject(nullptr), m_priceChangeId(priceChangeId), m_detailId(detailId),
    m_changeDate(changeDate), m_price(price), m_supplierId(supplierId)
{}

int PriceChange::priceChangeId() const { return m_priceChangeId; }
int PriceChange::detailId() const { return m_detailId; }
QDate PriceChange::changeDate() const { return m_changeDate; }
double PriceChange::price() const { return m_price; }
int PriceChange::supplierId() const { return m_supplierId; }

void PriceChange::setDetailId(int detailId) { m_detailId = detailId; }
void PriceChange::setChangeDate(const QDate &changeDate) { m_changeDate = changeDate; }
void PriceChange::setPrice(double price) { m_price = price; }
void PriceChange::setSupplierId(int supplierId) { m_supplierId = supplierId; }

bool PriceChange::save()
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("INSERT INTO auto_parts.price_changes "
                  "(detail_id, change_date, price, supplier_id) "
                  "VALUES (:detail_id, :change_date, :price, :supplier_id) "
                  "RETURNING price_change_id");
    query.bindValue(":detail_id", m_detailId);
    query.bindValue(":change_date", m_changeDate);
    query.bindValue(":price", m_price);
    query.bindValue(":supplier_id", m_supplierId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка сохранения изменения цены:" << query.lastError().text();
        return false;
    }
    m_priceChangeId = query.value(0).toInt();
    return true;
}

bool PriceChange::update()
{
    if (m_priceChangeId == 0) return false;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("UPDATE auto_parts.price_changes SET detail_id = :detail_id, "
                  "change_date = :change_date, price = :price, supplier_id = :supplier_id "
                  "WHERE price_change_id = :id");
    query.bindValue(":detail_id", m_detailId);
    query.bindValue(":change_date", m_changeDate);
    query.bindValue(":price", m_price);
    query.bindValue(":supplier_id", m_supplierId);
    query.bindValue(":id", m_priceChangeId);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления изменения цены:" << query.lastError().text();
        return false;
    }
    return true;
}

bool PriceChange::remove()
{
    if (m_priceChangeId == 0) return false;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("DELETE FROM auto_parts.price_changes WHERE price_change_id = :id");
    query.bindValue(":id", m_priceChangeId);
    if (!query.exec()) {
        qDebug() << "Ошибка удаления изменения цены:" << query.lastError().text();
        return false;
    }
    return true;
}

PriceChange* PriceChange::loadById(int priceChangeId)
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("SELECT price_change_id, detail_id, change_date, price, supplier_id "
                  "FROM auto_parts.price_changes WHERE price_change_id = :id");
    query.bindValue(":id", priceChangeId);
    if (!query.exec() || !query.next()) return nullptr;
    return new PriceChange(query.value("price_change_id").toInt(),
                           query.value("detail_id").toInt(),
                           query.value("change_date").toDate(),
                           query.value("price").toDouble(),
                           query.value("supplier_id").toInt());
}

QList<PriceChange*> PriceChange::loadAll()
{
    QList<PriceChange*> list;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.exec("SELECT price_change_id, detail_id, change_date, price, supplier_id "
               "FROM auto_parts.price_changes ORDER BY change_date DESC");
    while (query.next()) {
        list.append(new PriceChange(query.value("price_change_id").toInt(),
                                    query.value("detail_id").toInt(),
                                    query.value("change_date").toDate(),
                                    query.value("price").toDouble(),
                                    query.value("supplier_id").toInt()));
    }
    return list;
}

QList<PriceChange*> PriceChange::loadByDetail(int detailId)
{
    QList<PriceChange*> list;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("SELECT price_change_id, detail_id, change_date, price, supplier_id "
                  "FROM auto_parts.price_changes WHERE detail_id = :detail_id "
                  "ORDER BY change_date DESC");
    query.bindValue(":detail_id", detailId);
    if (!query.exec()) return list;
    while (query.next()) {
        list.append(new PriceChange(query.value("price_change_id").toInt(),
                                    query.value("detail_id").toInt(),
                                    query.value("change_date").toDate(),
                                    query.value("price").toDouble(),
                                    query.value("supplier_id").toInt()));
    }
    return list;
}