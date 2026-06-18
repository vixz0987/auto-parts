#include "accountingview.h"
#include "../Utils/database.h"
#include <QSqlQuery>

AccountingView::AccountingView(QObject *parent)
    : QObject(parent), m_quantity(0), m_unitPrice(0.0), m_totalAmount(0.0)
{}

AccountingView::AccountingView(const QDate &supplyDate, const QString &supplierName,
                               const QString &article, const QString &detailName,
                               int quantity, double unitPrice, double totalAmount)
    : QObject(nullptr), m_supplyDate(supplyDate), m_supplierName(supplierName),
    m_article(article), m_detailName(detailName), m_quantity(quantity),
    m_unitPrice(unitPrice), m_totalAmount(totalAmount)
{}

QDate AccountingView::supplyDate() const { return m_supplyDate; }
QString AccountingView::supplierName() const { return m_supplierName; }
QString AccountingView::article() const { return m_article; }
QString AccountingView::detailName() const { return m_detailName; }
int AccountingView::quantity() const { return m_quantity; }
double AccountingView::unitPrice() const { return m_unitPrice; }
double AccountingView::totalAmount() const { return m_totalAmount; }

QList<AccountingView*> AccountingView::loadAll()
{
    QList<AccountingView*> list;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.exec("SELECT supply_date, supplier_name, article, detail_name, "
               "quantity, unit_price, total_amount "
               "FROM auto_parts.vw_accounting_supplies ORDER BY supply_date DESC");
    while (query.next()) {
        list.append(new AccountingView(query.value("supply_date").toDate(),
                                       query.value("supplier_name").toString(),
                                       query.value("article").toString(),
                                       query.value("detail_name").toString(),
                                       query.value("quantity").toInt(),
                                       query.value("unit_price").toDouble(),
                                       query.value("total_amount").toDouble()));
    }
    return list;
}