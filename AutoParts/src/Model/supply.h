#ifndef SUPPLY_H
#define SUPPLY_H

#include <QObject>

class supply : public QObject
{
    Q_OBJECT
public:
    explicit supply(QObject *parent = nullptr);

signals:
};

#endif // SUPPLY_H
