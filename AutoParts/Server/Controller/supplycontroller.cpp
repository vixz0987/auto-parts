#include "supplycontroller.h"
#include "Model/supply.h"
#include "Model/supplyview.h"

QList<SupplyView*> SupplyController::getAllSupplies() {
    return SupplyView::loadAll();
}

bool SupplyController::addSupply(const QDate &supplyDate, int quantity,
                                 int supplierId, int priceChangeId)
{
    Supply *s = new Supply();
    s->setSupplyDate(supplyDate);
    s->setQuantity(quantity);
    s->setSupplierId(supplierId);
    s->setPriceChangeId(priceChangeId);
    bool ok = s->save();
    delete s;
    return ok;
}

bool SupplyController::updateSupply(int supplyId, const QDate &supplyDate,
                                    int quantity, int supplierId, int priceChangeId)
{
    Supply *s = Supply::loadById(supplyId);
    if (!s) return false;
    s->setSupplyDate(supplyDate);
    s->setQuantity(quantity);
    s->setSupplierId(supplierId);
    s->setPriceChangeId(priceChangeId);
    bool ok = s->update();
    delete s;
    return ok;
}

bool SupplyController::deleteSupply(int supplyId) {
    Supply *s = Supply::loadById(supplyId);
    if (!s) return false;
    bool ok = s->remove();
    delete s;
    return ok;
}
