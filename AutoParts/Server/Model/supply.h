#ifndef SUPPLY_H
#define SUPPLY_H

#include <QString>
#include <QDate>
#include <QObject>

class Supply : public QObject
{
    Q_OBJECT
public:
    explicit Supply(QObject *parent = nullptr);
    Supply(int supplyId, const QDate &supplyDate, int quantity, int priceChangeId);

    int supplyId() const;
    QDate supplyDate() const;
    int quantity() const;
    int priceChangeId() const;

    void setSupplyDate(const QDate &supplyDate);
    void setQuantity(int quantity);
    void setPriceChangeId(int priceChangeId);

    bool save();
    bool update();
    bool remove();
    static Supply* loadById(int supplyId);
    static QList<Supply*> loadAll();

private:
    int m_supplyId;
    QDate m_supplyDate;
    int m_quantity;
    int m_priceChangeId;
};

#endif // SUPPLY_H