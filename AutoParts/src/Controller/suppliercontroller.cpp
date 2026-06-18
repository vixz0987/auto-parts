#include "suppliercontroller.h"
#include "../Model/supplier.h"

QList<Supplier*> SupplierController::getAllSuppliers()
{
    return Supplier::loadAll();
}

bool SupplierController::addSupplier(const QString &name, const QString &phone,
                                     const QString &address)
{
    Supplier *s = new Supplier();
    s->setName(name);
    s->setPhone(phone);
    s->setAddress(address);
    bool result = s->save();
    delete s;
    return result;
}

bool SupplierController::updateSupplier(int supplierId, const QString &name,
                                        const QString &phone, const QString &address)
{
    Supplier *s = Supplier::loadById(supplierId);
    if (!s) return false;
    s->setName(name);
    s->setPhone(phone);
    s->setAddress(address);
    bool result = s->update();
    delete s;
    return result;
}

bool SupplierController::deleteSupplier(int supplierId)
{
    Supplier *s = Supplier::loadById(supplierId);
    if (!s) return false;
    bool result = s->remove();
    delete s;
    return result;
}