#include "pricechangecontroller.h"
#include "Model/pricechange.h"

QList<PriceChange*> PriceChangeController::getAllPriceChanges() {
    return PriceChange::loadAll();
}

bool PriceChangeController::addPriceChange(int detailId, const QDate &changeDate,
                                           double price, int supplierId)
{
    PriceChange *pc = new PriceChange();
    pc->setDetailId(detailId);
    pc->setChangeDate(changeDate);
    pc->setPrice(price);
    pc->setSupplierId(supplierId);
    bool ok = pc->save();
    delete pc;
    return ok;
}

bool PriceChangeController::updatePriceChange(int priceChangeId, int detailId,
                                              const QDate &changeDate, double price, int supplierId)
{
    PriceChange *pc = PriceChange::loadById(priceChangeId);
    if (!pc) return false;
    pc->setDetailId(detailId);
    pc->setChangeDate(changeDate);
    pc->setPrice(price);
    pc->setSupplierId(supplierId);
    bool ok = pc->update();
    delete pc;
    return ok;
}

bool PriceChangeController::deletePriceChange(int priceChangeId) {
    PriceChange *pc = PriceChange::loadById(priceChangeId);
    if (!pc) return false;
    bool ok = pc->remove();
    delete pc;
    return ok;
}