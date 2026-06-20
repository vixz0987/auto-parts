#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QList>
#include <QPair>

struct DetailItem { int id; QString article; QString name; };
struct SupplierItem { int id; QString name; };
struct PriceChangeItem { int id; int detailId; int supplierId; QString date; double price; };

// Данные для диалога поставки
struct SupplyDialogData {
    QList<SupplierItem> suppliers;               // все поставщики
    QList<DetailItem> details;                   // все детали
    QList<PriceChangeItem> priceChanges;         // кэш изменений цен
};

#endif // DATATYPES_H
