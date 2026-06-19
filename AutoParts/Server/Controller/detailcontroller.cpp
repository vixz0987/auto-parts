#include "detailcontroller.h"
#include "Model/detail.h"

QList<Detail*> DetailController::getAllDetails() {
    return Detail::loadAll();
}

bool DetailController::addDetail(const QString &article, const QString &name) {
    Detail *d = new Detail();
    d->setArticle(article);
    d->setName(name);
    bool ok = d->save();
    delete d;
    return ok;
}

bool DetailController::updateDetail(int detailId, const QString &article, const QString &name) {
    Detail *d = Detail::loadById(detailId);
    if (!d) return false;
    d->setArticle(article);
    d->setName(name);
    bool ok = d->update();
    delete d;
    return ok;
}

bool DetailController::deleteDetail(int detailId) {
    Detail *d = Detail::loadById(detailId);
    if (!d) return false;
    bool ok = d->remove();
    delete d;
    return ok;
}