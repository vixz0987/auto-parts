#include "tcpserver.h"
#include "Controller/authcontroller.h"
#include "Controller/usercontroller.h"
#include "Controller/suppliercontroller.h"
#include "Model/supplyview.h"
#include "Model/pricehistoryview.h"
#include "Model/accountingview.h"
#include "Model/userinfoview.h"
#include "Model/supplier.h"
#include "Model/user.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

static QJsonObject parseJsonArgs(const QString &args)
{
    int start = args.indexOf('{');
    int end = args.lastIndexOf('}');
    if (start < 0 || end < 0 || end <= start)
        return QJsonObject();
    QJsonDocument doc = QJsonDocument::fromJson(args.mid(start, end - start + 1).toUtf8());
    return doc.object();
}

TcpServer::TcpServer(quint16 port, QObject *parent)
    : QObject(parent), m_server(new QTcpServer(this))
{
    m_server->listen(QHostAddress::LocalHost, port);
    connect(m_server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
}

bool TcpServer::start()
{
    return m_server->isListening();
}

void TcpServer::onNewConnection()
{
    QTcpSocket *socket = m_server->nextPendingConnection();
    m_clients.append(socket);
    m_buffers.insert(socket, QByteArray());
    connect(socket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &TcpServer::onDisconnected);
}

void TcpServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    m_buffers[socket] += socket->readAll();
    QByteArray &buf = m_buffers[socket];
    while (buf.contains('\n')) {
        int idx = buf.indexOf('\n');
        QByteArray line = buf.left(idx).trimmed();
        buf.remove(0, idx + 1);
        if (line.isEmpty()) continue;
        QList<QByteArray> parts = line.split(' ');
        if (parts.size() < 2) continue;
        quint32 reqId = parts[0].toUInt();
        QString command = QString::fromUtf8(parts[1]);
        QString args;
        if (parts.size() > 2)
            args = QString::fromUtf8(line.mid(line.indexOf(' ', 2) + 1));

        QString response = processCommand(reqId, command, args);
        socket->write(response.toUtf8() + "\n");
        socket->flush();
    }
}

void TcpServer::onDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        m_clients.removeOne(socket);
        m_buffers.remove(socket);
        socket->deleteLater();
    }
}

QString TcpServer::processCommand(quint32 reqId, const QString &command, const QString &args)
{
    auto makeOk = [reqId](const QString &data) { return QString("%1 OK %2").arg(reqId).arg(data); };
    auto makeErr = [reqId](const QString &desc) { return QString("%1 ERROR %2").arg(reqId).arg(desc); };

    if (command == "LOGIN") {
        QStringList cred = args.split(' ');
        if (cred.size() < 2) return makeErr("Invalid format");
        User *user = AuthController::login(cred[0], cred[1]);
        if (user) {
            QJsonObject obj;
            obj["id"] = user->userId();
            obj["login"] = cred[0];
            obj["fio"] = user->fio();
            obj["role"] = user->role();
            QJsonDocument doc(obj);
            QString json = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
            delete user;
            return makeOk("LOGIN " + json);
        } else {
            return makeErr("Invalid credentials");
        }
    }
    else if (command == "ACTIVATE") {
        QJsonObject obj = parseJsonArgs(args);
        if (obj.isEmpty()) return makeErr("Invalid JSON");
        QString login = obj["login"].toString();
        QString fio = obj["fio"].toString();
        QString password = obj["password"].toString();
        if (login.isEmpty() || fio.isEmpty() || password.isEmpty()) return makeErr("Missing fields");
        bool ok = AuthController::activateUser(login, fio, password);
        return ok ? makeOk("ACTIVATED") : makeErr("Activation failed");
    }
    else if (command == "USERS") {
        QList<UserInfoView*> users = UserController::getAllUsers();
        QJsonArray arr;
        for (auto *u : users) {
            QJsonObject obj;
            obj["id"] = u->userId();
            obj["login"] = u->login();
            obj["fio"] = u->fio();
            obj["role"] = u->role();
            arr.append(obj);
        }
        qDeleteAll(users);
        QString json = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
        return makeOk("USERS " + json);
    }
    else if (command == "ADDUSER") {
        QJsonObject obj = parseJsonArgs(args);
        if (obj.isEmpty()) return makeErr("Invalid JSON");
        QString login = obj["login"].toString();
        QString fio = obj["fio"].toString();
        QString role = obj["role"].toString();
        if (login.isEmpty() || fio.isEmpty() || role.isEmpty()) return makeErr("Missing fields");
        bool ok = UserController::createUser(login, fio, role);
        return ok ? makeOk("USERADDED") : makeErr("Add user failed");
    }
    else if (command == "CHANGEROLE") {
        QJsonObject obj = parseJsonArgs(args);
        if (obj.isEmpty()) return makeErr("Invalid JSON");
        int userId = obj["userId"].toInt();
        QString role = obj["role"].toString();
        if (userId <= 0 || role.isEmpty()) return makeErr("Invalid fields");
        bool ok = UserController::changeRole(userId, role);
        return ok ? makeOk("ROLECHANGED") : makeErr("Change role failed");
    }
    else if (command == "DELUSER") {
        int userId = 0;
        QJsonObject obj = parseJsonArgs(args);
        if (!obj.isEmpty()) {
            userId = obj["userId"].toInt();
        } else {
            userId = args.trimmed().toInt();
        }
        if (userId <= 0) return makeErr("Invalid user id");
        bool ok = UserController::deleteUser(userId);
        return ok ? makeOk("USERDELETED") : makeErr("Delete user failed: user not found or DB error");
    }
    else if (command == "SUPPLIERS") {
        QList<Supplier*> suppliers = SupplierController::getAllSuppliers();
        QJsonArray arr;
        for (auto *s : suppliers) {
            QJsonObject obj;
            obj["id"] = s->supplierId();
            obj["name"] = s->name();
            obj["phone"] = s->phone();
            obj["address"] = s->address();
            arr.append(obj);
        }
        qDeleteAll(suppliers);
        QString json = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
        return makeOk("SUPPLIERS " + json);
    }
    else if (command == "ADDSUPPLIER") {
        QJsonObject obj = parseJsonArgs(args);
        if (obj.isEmpty()) return makeErr("Invalid JSON");
        QString name = obj["name"].toString();
        QString phone = obj["phone"].toString();
        QString address = obj["address"].toString();
        if (name.isEmpty() || phone.isEmpty()) return makeErr("Missing fields");
        bool ok = SupplierController::addSupplier(name, phone, address);
        return ok ? makeOk("SUPPLIERADDED") : makeErr("Add supplier failed");
    }
    else if (command == "UPDSUPPLIER") {
        QJsonObject obj = parseJsonArgs(args);
        if (obj.isEmpty()) return makeErr("Invalid JSON");
        int id = obj["id"].toInt();
        QString name = obj["name"].toString();
        QString phone = obj["phone"].toString();
        QString address = obj["address"].toString();
        if (id <= 0 || name.isEmpty() || phone.isEmpty()) return makeErr("Invalid fields");
        bool ok = SupplierController::updateSupplier(id, name, phone, address);
        return ok ? makeOk("SUPPLIERUPDATED") : makeErr("Update supplier failed");
    }
    else if (command == "DELSUPPLIER") {
        int supplierId = 0;
        QJsonObject obj = parseJsonArgs(args);
        if (!obj.isEmpty()) {
            supplierId = obj["supplierId"].toInt();
        } else {
            supplierId = args.trimmed().toInt();
        }
        if (supplierId <= 0) return makeErr("Invalid supplier id");
        bool ok = SupplierController::deleteSupplier(supplierId);
        return ok ? makeOk("SUPPLIERDELETED") : makeErr("Delete supplier failed: supplier not found or DB error");
    }
    else if (command == "SUPPLIES") {
        QList<SupplyView*> supplies = SupplyView::loadAll();
        QJsonArray arr;
        for (auto *v : supplies) {
            QJsonObject obj;
            obj["supply_id"] = v->supplyId();
            obj["supply_date"] = v->supplyDate().toString(Qt::ISODate);
            obj["supplier_name"] = v->supplierName();
            obj["article"] = v->article();
            obj["detail_name"] = v->detailName();
            obj["price"] = v->price();
            obj["quantity"] = v->quantity();
            obj["total_amount"] = v->totalAmount();
            arr.append(obj);
        }
        qDeleteAll(supplies);
        QString json = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
        return makeOk("SUPPLIES " + json);
    }
    else if (command == "PRICEHISTORY") {
        QList<PriceHistoryView*> prices = PriceHistoryView::loadAll();
        QJsonArray arr;
        for (auto *ph : prices) {
            QJsonObject obj;
            obj["article"] = ph->article();
            obj["detail_name"] = ph->detailName();
            obj["change_date"] = ph->changeDate().toString(Qt::ISODate);
            obj["price"] = ph->price();
            arr.append(obj);
        }
        qDeleteAll(prices);
        QString json = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
        return makeOk("PRICEHISTORY " + json);
    }
    else if (command == "ACCOUNTING") {
        QList<AccountingView*> acc = AccountingView::loadAll();
        QJsonArray arr;
        for (auto *a : acc) {
            QJsonObject obj;
            obj["supply_date"] = a->supplyDate().toString(Qt::ISODate);
            obj["supplier_name"] = a->supplierName();
            obj["article"] = a->article();
            obj["detail_name"] = a->detailName();
            obj["quantity"] = a->quantity();
            obj["unit_price"] = a->unitPrice();
            obj["total_amount"] = a->totalAmount();
            arr.append(obj);
        }
        qDeleteAll(acc);
        QString json = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
        return makeOk("ACCOUNTING " + json);
    }

    return makeErr("Unknown command");
}