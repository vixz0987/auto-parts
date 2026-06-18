#ifndef ACCOUNTINGVIEW_H
#define ACCOUNTINGVIEW_H

#include <QObject>

class accountingview : public QObject
{
    Q_OBJECT
public:
    explicit accountingview(QObject *parent = nullptr);

signals:
};

#endif // ACCOUNTINGVIEW_H
