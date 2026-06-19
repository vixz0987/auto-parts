#include "pricechangecontroller.h"
#include "Model/pricechange.h"
#include "Model/supplierpricechange.h"

QList<PriceChange*> PriceChangeController::getAllPriceChanges() {
    return PriceChange::loadAll();
}

bool PriceChangeController::addPriceChange(int detailId, const QDate &changeDate,
                                           double price, const QList<int> &supplierIds)
{
    PriceChange *pc = new PriceChange();
    pc->setDetailId(detailId);
    pc->setChangeDate(changeDate);
    pc->setPrice(price);
    if (!pc->save()) {
        delete pc;
        return false;
    }
    int pcId = pc->priceChangeId();
    delete pc;

    for (int sid : supplierIds) {
        SupplierPriceChange::link(sid, pcId);
    }
    return true;
}

bool PriceChangeController::updatePriceChange(int priceChangeId, int detailId,
                                              const QDate &changeDate, double price)
{
    PriceChange *pc = PriceChange::loadById(priceChangeId);
    if (!pc) return false;
    pc->setDetailId(detailId);
    pc->setChangeDate(changeDate);
    pc->setPrice(price);
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
