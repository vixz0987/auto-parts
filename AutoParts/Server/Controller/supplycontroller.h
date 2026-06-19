#ifndef SUPPLYCONTROLLER_H
#define SUPPLYCONTROLLER_H

#include <QList>
#include <QDate>
class SupplyView;

class SupplyController
{
public:
    static QList<SupplyView*> getAllSupplies();
    static bool addSupply(const QDate &supplyDate, int quantity,
                          int supplierId, int priceChangeId);
    static bool updateSupply(int supplyId, const QDate &supplyDate, int quantity,
                             int supplierId, int priceChangeId);
    static bool deleteSupply(int supplyId);
};

#endif // SUPPLYCONTROLLER_H
