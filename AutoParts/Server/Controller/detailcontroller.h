#ifndef DETAILCONTROLLER_H
#define DETAILCONTROLLER_H

#include <QList>
class Detail;

class DetailController
{
public:
    static QList<Detail*> getAllDetails();
    static bool addDetail(const QString &article, const QString &name);
    static bool updateDetail(int detailId, const QString &article, const QString &name);
    static bool deleteDetail(int detailId);
};

#endif // DETAILCONTROLLER_H