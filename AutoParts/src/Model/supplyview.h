#ifndef SUPPLYVIEW_H
#define SUPPLYVIEW_H

#include <QString>
#include <QDate>
#include <QObject>

// Класс для представления vw_supply_manager (менеджер по поставкам)
class SupplyView : public QObject
{
    Q_OBJECT

public:
    explicit SupplyView(QObject *parent = nullptr);
    SupplyView(int supplyId, const QDate &supplyDate, const QString &supplierName,
               const QString &article, const QString &detailName,
               double price, int quantity, double totalAmount);

    int supplyId() const;
    QDate supplyDate() const;
    QString supplierName() const;
    QString article() const;
    QString detailName() const;
    double price() const;
    int quantity() const;
    double totalAmount() const;

    static QList<SupplyView*> loadAll();

private:
    int m_supplyId;
    QDate m_supplyDate;
    QString m_supplierName;
    QString m_article;
    QString m_detailName;
    double m_price;
    int m_quantity;
    double m_totalAmount;
};

#endif // SUPPLYVIEW_H