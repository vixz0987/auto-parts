#include "userinfoview.h"
#include "../Utils/database.h"
#include <QSqlQuery>

UserInfoView::UserInfoView(QObject *parent)
    : QObject(parent), m_userId(0)
{}

UserInfoView::UserInfoView(int userId, const QString &login,
                           const QString &fio, const QString &role)
    : QObject(nullptr), m_userId(userId), m_login(login),
    m_fio(fio), m_role(role)
{}

int UserInfoView::userId() const { return m_userId; }
QString UserInfoView::login() const { return m_login; }
QString UserInfoView::fio() const { return m_fio; }
QString UserInfoView::role() const { return m_role; }

QList<UserInfoView*> UserInfoView::loadAll()
{
    QList<UserInfoView*> list;
    QSqlDatabase db = Database::instance()->getDb();
    QSqlQuery query(db);
    query.exec("SELECT user_id, login, fio, role "
               "FROM auto_parts.vw_users_info ORDER BY user_id");
    while (query.next()) {
        list.append(new UserInfoView(query.value("user_id").toInt(),
                                     query.value("login").toString(),
                                     query.value("fio").toString(),
                                     query.value("role").toString()));
    }
    return list;
}