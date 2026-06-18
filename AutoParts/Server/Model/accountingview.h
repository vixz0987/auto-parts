#ifndef ACCOUNTINGVIEW_H
#define ACCOUNTINGVIEW_H

#include <QString>
#include <QDate>
#include <QObject>

// Класс для представления vw_accounting_supplies (бухгалтер)
class AccountingView : public QObject
{
    Q_OBJECT

public:
    explicit AccountingView(QObject *parent = nullptr);
    AccountingView(const QDate &supplyDate, const QString &supplierName,
                   const QString &article, const QString &detailName,
                   int quantity, double unitPrice, double totalAmount);

    QDate supplyDate() const;
    QString supplierName() const;
    QString article() const;
    QString detailName() const;
    int quantity() const;
    double unitPrice() const;
    double totalAmount() const;

    static QList<AccountingView*> loadAll();

private:
    QDate m_supplyDate;
    QString m_supplierName;
    QString m_article;
    QString m_detailName;
    int m_quantity;
    double m_unitPrice;
    double m_totalAmount;
};

#endif // ACCOUNTINGVIEW_H