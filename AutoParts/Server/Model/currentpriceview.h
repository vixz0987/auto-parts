#ifndef CURRENTPRICEVIEW_H
#define CURRENTPRICEVIEW_H

#include <QObject>
#include <QString>
#include <QDate>

class CurrentPriceView : public QObject
{
    Q_OBJECT
public:
    explicit CurrentPriceView(QObject *parent = nullptr);
    CurrentPriceView(int detailId, const QString &article, const QString &detailName,
                     double price, const QDate &changeDate,
                     int supplierId, const QString &supplierName);

    int detailId() const;
    QString article() const;
    QString detailName() const;
    double price() const;
    QDate changeDate() const;
    int supplierId() const;
    QString supplierName() const;

    static QList<CurrentPriceView*> loadAll();

private:
    int m_detailId;
    QString m_article;
    QString m_detailName;
    double m_price;
    QDate m_changeDate;
    int m_supplierId;
    QString m_supplierName;
};

#endif // CURRENTPRICEVIEW_H