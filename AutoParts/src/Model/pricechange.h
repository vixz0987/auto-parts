#ifndef PRICECHANGE_H
#define PRICECHANGE_H

#include <QObject>

class pricechange : public QObject
{
    Q_OBJECT
public:
    explicit pricechange(QObject *parent = nullptr);

signals:
};

#endif // PRICECHANGE_H
