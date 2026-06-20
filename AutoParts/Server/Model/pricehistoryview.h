#ifndef PRICEHISTORYVIEW_H
#define PRICEHISTORYVIEW_H

#include <QString>
#include <QDate>
#include <QObject>

// Класс для представления vw_price_history (история цен)
class PriceHistoryView : public QObject
{
    Q_OBJECT
public:
    explicit PriceHistoryView(QObject *parent = nullptr);
    PriceHistoryView(const QString &article, const QString &detailName,
                     const QString &supplierName,
                     const QDate &changeDate, double price);

    QString article() const;
    QString detailName() const;
    QString supplierName() const;
    QDate changeDate() const;
    double price() const;

    static QList<PriceHistoryView*> loadAll();

private:
    QString m_article;
    QString m_detailName;
    QString m_supplierName;
    QDate m_changeDate;
    double m_price;
};

#endif // PRICEHISTORYVIEW_H