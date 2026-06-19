#ifndef PRICECHANGECONTROLLER_H
#define PRICECHANGECONTROLLER_H

#include <QList>
#include <QDate>
class PriceChange;

class PriceChangeController
{
public:
    static QList<PriceChange*> getAllPriceChanges();
    static bool addPriceChange(int detailId, const QDate &changeDate, double price,
                               const QList<int> &supplierIds);
    static bool updatePriceChange(int priceChangeId, int detailId,
                                  const QDate &changeDate, double price);
    static bool deletePriceChange(int priceChangeId);
};

#endif // PRICECHANGECONTROLLER_H