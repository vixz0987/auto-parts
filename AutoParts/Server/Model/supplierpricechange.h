#ifndef SUPPLIERPRICECHANGE_H
#define SUPPLIERPRICECHANGE_H

#include <QObject>

// Модель для связующей таблицы auto_parts.supplier_price_changes (M:M)
class SupplierPriceChange : public QObject
{
    Q_OBJECT

public:
    explicit SupplierPriceChange(QObject *parent = nullptr);
    SupplierPriceChange(int supplierId, int priceChangeId);

    int supplierId() const;
    int priceChangeId() const;

    // Статические методы для работы со связью
    static bool link(int supplierId, int priceChangeId);    // создать связь
    static bool unlink(int supplierId, int priceChangeId);  // удалить связь
    static bool exists(int supplierId, int priceChangeId);  // проверить наличие
    static QList<int> priceChangeIdsForSupplier(int supplierId); // все изменения поставщика

private:
    int m_supplierId;
    int m_priceChangeId;
};

#endif // SUPPLIERPRICECHANGE_H
