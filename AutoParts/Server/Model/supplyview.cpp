#include "supplyview.h"
#include "Utils/database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

SupplyView::SupplyView(QObject *parent)
    : QObject(parent), m_supplyId(0), m_price(0.0), m_quantity(0), m_totalAmount(0.0)
{}

SupplyView::SupplyView(int supplyId, const QDate &supplyDate, const QString &supplierName,
                       const QString &article, const QString &detailName,
                       double price, int quantity, double totalAmount)
    : QObject(nullptr), m_supplyId(supplyId), m_supplyDate(supplyDate),
    m_supplierName(supplierName), m_article(article), m_detailName(detailName),
    m_price(price), m_quantity(quantity), m_totalAmount(totalAmount)
{}

int SupplyView::supplyId() const { return m_supplyId; }
QDate SupplyView::supplyDate() const { return m_supplyDate; }
QString SupplyView::supplierName() const { return m_supplierName; }
QString SupplyView::article() const { return m_article; }
QString SupplyView::detailName() const { return m_detailName; }
double SupplyView::price() const { return m_price; }
int SupplyView::quantity() const { return m_quantity; }
double SupplyView::totalAmount() const { return m_totalAmount; }

// Загрузка всех записей из представления
QList<SupplyView*> SupplyView::loadAll()
{
    QList<SupplyView*> list;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.exec("SELECT supply_id, supply_date, supplier_name, article, "
               "detail_name, price, quantity, total_amount "
               "FROM auto_parts.vw_supply_manager ORDER BY supply_date DESC");
    while (query.next()) {
        list.append(new SupplyView(query.value("supply_id").toInt(),
                                   query.value("supply_date").toDate(),
                                   query.value("supplier_name").toString(),
                                   query.value("article").toString(),
                                   query.value("detail_name").toString(),
                                   query.value("price").toDouble(),
                                   query.value("quantity").toInt(),
                                   query.value("total_amount").toDouble()));
    }
    return list;
}