#include "currentpriceview.h"
#include "Utils/database.h"
#include <QSqlQuery>

CurrentPriceView::CurrentPriceView(QObject *parent)
    : QObject(parent), m_detailId(0), m_price(0.0), m_supplierId(0)
{}

CurrentPriceView::CurrentPriceView(int detailId, const QString &article,
                                   const QString &detailName, double price,
                                   const QDate &changeDate, int supplierId,
                                   const QString &supplierName)
    : QObject(nullptr), m_detailId(detailId), m_article(article),
    m_detailName(detailName), m_price(price), m_changeDate(changeDate),
    m_supplierId(supplierId), m_supplierName(supplierName)
{}

int CurrentPriceView::detailId() const { return m_detailId; }
QString CurrentPriceView::article() const { return m_article; }
QString CurrentPriceView::detailName() const { return m_detailName; }
double CurrentPriceView::price() const { return m_price; }
QDate CurrentPriceView::changeDate() const { return m_changeDate; }
int CurrentPriceView::supplierId() const { return m_supplierId; }
QString CurrentPriceView::supplierName() const { return m_supplierName; }

QList<CurrentPriceView*> CurrentPriceView::loadAll()
{
    QList<CurrentPriceView*> list;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.exec("SELECT detail_id, article, name, price, change_date, supplier_id, supplier_name "
               "FROM auto_parts.vw_current_prices ORDER BY article");
    while (query.next()) {
        list.append(new CurrentPriceView(
            query.value("detail_id").toInt(),
            query.value("article").toString(),
            query.value("name").toString(),
            query.value("price").toDouble(),
            query.value("change_date").toDate(),
            query.value("supplier_id").toInt(),
            query.value("supplier_name").toString()
            ));
    }
    return list;
}