#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QList>
#include <QPair>

struct DetailItem { int id; QString article; QString name; };
struct SupplierItem { int id; QString name; };
struct PriceChangeItem { int id; int detailId; QString date; double price; };

struct SupplyDialogData {
    QList<QPair<int, QString>> suppliers;      // id, name
    QList<QPair<int, QString>> priceChanges;   // id, описание
};

#endif // DATATYPES_H
