#ifndef PRICECHANGE_H
#define PRICECHANGE_H

#include <QString>
#include <QDate>
#include <QObject>

class PriceChange : public QObject
{
    Q_OBJECT
public:
    explicit PriceChange(QObject *parent = nullptr);
    PriceChange(int priceChangeId, int detailId, const QDate &changeDate,
                double price, int supplierId);

    int priceChangeId() const;
    int detailId() const;
    QDate changeDate() const;
    double price() const;
    int supplierId() const;

    void setDetailId(int detailId);
    void setChangeDate(const QDate &changeDate);
    void setPrice(double price);
    void setSupplierId(int supplierId);

    bool save();
    bool update();
    bool remove();
    static PriceChange* loadById(int priceChangeId);
    static QList<PriceChange*> loadAll();
    static QList<PriceChange*> loadByDetail(int detailId);

private:
    int m_priceChangeId;
    int m_detailId;
    QDate m_changeDate;
    double m_price;
    int m_supplierId;
};

#endif // PRICECHANGE_H