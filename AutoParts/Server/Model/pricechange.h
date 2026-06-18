#ifndef PRICECHANGE_H
#define PRICECHANGE_H

#include <QString>
#include <QDate>
#include <QObject>

// Класс-модель для таблицы auto_parts.price_changes (Изменения цен)
class PriceChange : public QObject
{
    Q_OBJECT

public:
    explicit PriceChange(QObject *parent = nullptr);
    PriceChange(int priceChangeId, int detailId, const QDate &changeDate, double price);

    int priceChangeId() const;
    int detailId() const;
    QDate changeDate() const;
    double price() const;

    void setDetailId(int detailId);
    void setChangeDate(const QDate &changeDate);
    void setPrice(double price);

    // CRUD
    bool save();
    bool update();
    bool remove();
    static PriceChange* loadById(int priceChangeId);
    static QList<PriceChange*> loadAll();
    static QList<PriceChange*> loadByDetail(int detailId);  // все изменения по детали

private:
    int m_priceChangeId;
    int m_detailId;
    QDate m_changeDate;
    double m_price;
};

#endif // PRICECHANGE_H