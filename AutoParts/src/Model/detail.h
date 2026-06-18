#ifndef DETAIL_H
#define DETAIL_H

#include <QObject>

class detail : public QObject
{
    Q_OBJECT
public:
    explicit detail(QObject *parent = nullptr);

signals:
};

#endif // DETAIL_H
