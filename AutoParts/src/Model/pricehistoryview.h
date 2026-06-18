#ifndef PRICEHISTORYVIEW_H
#define PRICEHISTORYVIEW_H

#include <QObject>

class pricehistoryview : public QObject
{
    Q_OBJECT
public:
    explicit pricehistoryview(QObject *parent = nullptr);

signals:
};

#endif // PRICEHISTORYVIEW_H
