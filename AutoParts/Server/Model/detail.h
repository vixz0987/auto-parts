#ifndef DETAIL_H
#define DETAIL_H

#include <QString>
#include <QObject>

// Класс-модель для таблицы auto_parts.details (Детали)
class Detail : public QObject
{
    Q_OBJECT

public:
    explicit Detail(QObject *parent = nullptr);
    Detail(int detailId, const QString &article, const QString &name);

    int detailId() const;
    QString article() const;
    QString name() const;

    void setArticle(const QString &article);
    void setName(const QString &name);

    // CRUD
    bool save();
    bool update();
    bool remove();
    static Detail* loadById(int detailId);
    static QList<Detail*> loadAll();

private:
    int m_detailId;
    QString m_article;
    QString m_name;
};

#endif // DETAIL_H
