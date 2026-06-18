#include "detail.h"
#include "Utils/database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Detail::Detail(QObject *parent)
    : QObject(parent), m_detailId(0)
{}

Detail::Detail(int detailId, const QString &article, const QString &name)
    : QObject(nullptr), m_detailId(detailId), m_article(article), m_name(name)
{}

int Detail::detailId() const { return m_detailId; }
QString Detail::article() const { return m_article; }
QString Detail::name() const { return m_name; }

void Detail::setArticle(const QString &article) { m_article = article; }
void Detail::setName(const QString &name) { m_name = name; }

// Сохранение новой детали
bool Detail::save()
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("INSERT INTO auto_parts.details (article, name) "
                  "VALUES (:article, :name) RETURNING detail_id");
    query.bindValue(":article", m_article);
    query.bindValue(":name", m_name);

    if (!query.exec() || !query.next()) {
        qDebug() << "Ошибка сохранения детали:" << query.lastError().text();
        return false;
    }
    m_detailId = query.value(0).toInt();
    return true;
}

// Обновление детали
bool Detail::update()
{
    if (m_detailId == 0) return false;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("UPDATE auto_parts.details SET article = :article, "
                  "name = :name WHERE detail_id = :id");
    query.bindValue(":article", m_article);
    query.bindValue(":name", m_name);
    query.bindValue(":id", m_detailId);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления детали:" << query.lastError().text();
        return false;
    }
    return true;
}

// Удаление детали
bool Detail::remove()
{
    if (m_detailId == 0) return false;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("DELETE FROM auto_parts.details WHERE detail_id = :id");
    query.bindValue(":id", m_detailId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления детали:" << query.lastError().text();
        return false;
    }
    return true;
}

// Загрузка детали по идентификатору
Detail* Detail::loadById(int detailId)
{
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.prepare("SELECT detail_id, article, name "
                  "FROM auto_parts.details WHERE detail_id = :id");
    query.bindValue(":id", detailId);
    if (!query.exec() || !query.next())
        return nullptr;
    return new Detail(query.value("detail_id").toInt(),
                      query.value("article").toString(),
                      query.value("name").toString());
}

// Загрузка всех деталей
QList<Detail*> Detail::loadAll()
{
    QList<Detail*> details;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.exec("SELECT detail_id, article, name "
               "FROM auto_parts.details ORDER BY detail_id");
    while (query.next()) {
        details.append(new Detail(query.value("detail_id").toInt(),
                                  query.value("article").toString(),
                                  query.value("name").toString()));
    }
    return details;
}