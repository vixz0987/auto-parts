#ifndef SUPPLIER_H
#define SUPPLIER_H

#include <QObject>

class supplier : public QObject
{
    Q_OBJECT
public:
    explicit supplier(QObject *parent = nullptr);

signals:
};

#endif // SUPPLIER_H
