#ifndef USERINFOVIEW_H
#define USERINFOVIEW_H

#include <QObject>

class userinfoview : public QObject
{
    Q_OBJECT
public:
    explicit userinfoview(QObject *parent = nullptr);

signals:
};

#endif // USERINFOVIEW_H
