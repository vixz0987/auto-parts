#include "pricehistoryview.h"
#include "../Utils/database.h"
#include <QSqlQuery>

PriceHistoryView::PriceHistoryView(QObject *parent)
    : QObject(parent), m_price(0.0)
{}

PriceHistoryView::PriceHistoryView(const QString &article, const QString &detailName,
                                   const QDate &changeDate, double price)
    : QObject(nullptr), m_article(article), m_detailName(detailName),
    m_changeDate(changeDate), m_price(price)
{}

QString PriceHistoryView::article() const { return m_article; }
QString PriceHistoryView::detailName() const { return m_detailName; }
QDate PriceHistoryView::changeDate() const { return m_changeDate; }
double PriceHistoryView::price() const { return m_price; }

QList<PriceHistoryView*> PriceHistoryView::loadAll()
{
    QList<PriceHistoryView*> list;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.exec("SELECT article, detail_name, change_date, price "
               "FROM auto_parts.vw_price_history ORDER BY change_date DESC");
    while (query.next()) {
        list.append(new PriceHistoryView(query.value("article").toString(),
                                         query.value("detail_name").toString(),
                                         query.value("change_date").toDate(),
                                         query.value("price").toDouble()));
    }
    return list;
}