#include "ClientService.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

ClientService::ClientService(TcpClient* client, QObject* parent)
    : QObject(parent), m_client(client)
{
    connect(m_client, &TcpClient::responseReceived, this, &ClientService::onResponse);
}

quint32 ClientService::sendCommand(const QString& cmd)
{
    return m_client->sendCommand(cmd);
}

QJsonDocument ClientService::parseJsonPayload(const QString& response, const QString& expectedPrefix)
{
    if (!response.startsWith(expectedPrefix))
        return QJsonDocument();
    QString jsonStr = response.mid(expectedPrefix.length());
    return QJsonDocument::fromJson(jsonStr.toUtf8());
}

// ---------- Аутентификация ----------
void ClientService::login(const QString& login, const QString& password)
{
    m_loginRequestId = sendCommand(QString("LOGIN %1 %2").arg(login, password));
}

void ClientService::activate(const QString& login, const QString& fio, const QString& password)
{
    QJsonObject obj;
    obj["login"] = login;
    obj["fio"] = fio;
    obj["password"] = password;
    QJsonDocument doc(obj);
    m_activationRequestId = sendCommand("ACTIVATE " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

// ---------- Пользователи ----------
void ClientService::fetchUsers()
{
    m_usersFetchId = sendCommand("USERS");
}

void ClientService::addUser(const QString& login, const QString& fio, const QString& role)
{
    QJsonObject obj;
    obj["login"] = login;
    obj["fio"] = fio;
    obj["role"] = role;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("ADDUSER " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::changeRole(int userId, const QString& newRole)
{
    QJsonObject obj;
    obj["userId"] = userId;
    obj["role"] = newRole;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("CHANGEROLE " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::deleteUser(int userId)
{
    QJsonObject obj;
    obj["userId"] = userId;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("DELUSER " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::changePassword(int userId, const QString& oldPassword, const QString& newPassword)
{
    QJsonObject obj;
    obj["userId"] = userId;
    obj["oldPassword"] = oldPassword;
    obj["newPassword"] = newPassword;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("CHANGEPASSWORD " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::changeFio(int userId, const QString& newFio)
{
    QJsonObject obj;
    obj["userId"] = userId;
    obj["newFio"] = newFio;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("CHANGEFIO " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

// ---------- Поставщики ----------
void ClientService::fetchSuppliers()
{
    m_suppliersFetchId = sendCommand("SUPPLIERS");
}

void ClientService::addSupplier(const QString& name, const QString& phone, const QString& address)
{
    QJsonObject obj;
    obj["name"] = name;
    obj["phone"] = phone;
    obj["address"] = address;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("ADDSUPPLIER " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::updateSupplier(int id, const QString& name, const QString& phone, const QString& address)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["phone"] = phone;
    obj["address"] = address;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("UPDSUPPLIER " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::deleteSupplier(int id)
{
    QJsonObject obj;
    obj["supplierId"] = id;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("DELSUPPLIER " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

// ---------- Детали ----------
void ClientService::fetchDetails()
{
    m_detailsFetchId = sendCommand("DETAILS");
}

void ClientService::addDetail(const QString& article, const QString& name)
{
    QJsonObject obj;
    obj["article"] = article;
    obj["name"] = name;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("ADDDETAIL " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::updateDetail(int id, const QString& article, const QString& name)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["article"] = article;
    obj["name"] = name;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("UPDDETAIL " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::deleteDetail(int id)
{
    QJsonObject obj;
    obj["detailId"] = id;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("DELDETAIL " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

// ---------- Изменения цен ----------
void ClientService::fetchPriceChanges()
{
    m_priceChangesFetchId = sendCommand("PRICECHANGES");
}

void ClientService::addPriceChange(int detailId, const QDate& changeDate, double price, int supplierId)
{
    QJsonObject obj;
    obj["detail_id"] = detailId;
    obj["change_date"] = changeDate.toString(Qt::ISODate);
    obj["price"] = price;
    obj["supplier_id"] = supplierId;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("ADD_PRICE_CHANGE " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::updatePriceChange(int id, int detailId, const QDate& changeDate, double price, int supplierId)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["detail_id"] = detailId;
    obj["change_date"] = changeDate.toString(Qt::ISODate);
    obj["price"] = price;
    obj["supplier_id"] = supplierId;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("UPD_PRICE_CHANGE " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::deletePriceChange(int id)
{
    QJsonObject obj;
    obj["price_change_id"] = id;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("DEL_PRICE_CHANGE " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

// ---------- Поставки ----------
void ClientService::fetchSupplies()
{
    m_suppliesFetchId = sendCommand("SUPPLIES");
}

void ClientService::addSupply(const QDate& supplyDate, int quantity, int priceChangeId)
{
    QJsonObject obj;
    obj["supply_date"] = supplyDate.toString(Qt::ISODate);
    obj["quantity"] = quantity;
    obj["price_change_id"] = priceChangeId;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("ADDSUPPLY " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::updateSupply(int supplyId, const QDate& supplyDate, int quantity, int priceChangeId)
{
    QJsonObject obj;
    obj["supply_id"] = supplyId;
    obj["supply_date"] = supplyDate.toString(Qt::ISODate);
    obj["quantity"] = quantity;
    obj["price_change_id"] = priceChangeId;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("UPDSUPPLY " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void ClientService::deleteSupply(int supplyId)
{
    QJsonObject obj;
    obj["supply_id"] = supplyId;
    QJsonDocument doc(obj);
    m_operationRequestId = sendCommand("DELSUPPLY " + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

// ---------- Представления ----------
void ClientService::fetchCurrentPrices()
{
    m_currentPricesFetchId = sendCommand("CURRENTPRICES");
}

void ClientService::fetchPriceHistory()
{
    m_priceHistoryFetchId = sendCommand("PRICEHISTORY");
}

void ClientService::fetchAccounting()
{
    m_accountingFetchId = sendCommand("ACCOUNTING");
}

// ---------- Обработка ответов ----------
void ClientService::onResponse(quint32 id, const QString& response)
{
    if (id == m_loginRequestId) {
        m_loginRequestId = 0;
        handleLoginResponse(response);
        return;
    }
    if (id == m_activationRequestId) {
        m_activationRequestId = 0;
        handleActivationResponse(response);
        return;
    }
    if (id == m_usersFetchId) {
        m_usersFetchId = 0;
        handleUsersResponse(response);
        return;
    }
    if (id == m_suppliersFetchId) {
        m_suppliersFetchId = 0;
        handleSuppliersResponse(response);
        return;
    }
    if (id == m_detailsFetchId) {
        m_detailsFetchId = 0;
        handleDetailsResponse(response);
        return;
    }
    if (id == m_priceChangesFetchId) {
        m_priceChangesFetchId = 0;
        handlePriceChangesResponse(response);
        return;
    }
    if (id == m_suppliesFetchId) {
        m_suppliesFetchId = 0;
        handleSuppliesResponse(response);
        return;
    }
    if (id == m_currentPricesFetchId) {
        m_currentPricesFetchId = 0;
        handleCurrentPricesResponse(response);
        return;
    }
    if (id == m_priceHistoryFetchId) {
        m_priceHistoryFetchId = 0;
        handlePriceHistoryResponse(response);
        return;
    }
    if (id == m_accountingFetchId) {
        m_accountingFetchId = 0;
        handleAccountingResponse(response);
        return;
    }
    if (id == m_operationRequestId) {
        m_operationRequestId = 0;
        handleOperationResponse(response);
        return;
    }
    qWarning() << "Unknown response id:" << id;
}

void ClientService::handleLoginResponse(const QString& response)
{
    if (response.startsWith("OK LOGIN ")) {
        QJsonDocument doc = parseJsonPayload(response, "OK LOGIN ");
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            UserData user;
            user.id = obj["id"].toInt();
            user.login = obj["login"].toString();
            user.fio = obj["fio"].toString();
            user.role = obj["role"].toString();
            emit loginSuccess(user);
            return;
        }
    }
    QString err = "Неверный логин или пароль";
    if (response.startsWith("ERROR ")) err = response.mid(6);
    emit loginError(err);
}

void ClientService::handleActivationResponse(const QString& response)
{
    if (response.startsWith("OK ACTIVATED")) {
        emit activationSuccess();
    } else {
        QString err = "Ошибка активации";
        if (response.startsWith("ERROR ")) err = response.mid(6);
        emit activationError(err);
    }
}

void ClientService::handleUsersResponse(const QString& response)
{
    QJsonDocument doc = parseJsonPayload(response, "OK USERS ");
    if (doc.isNull() || !doc.isArray()) {
        emit operationError(0, "Ошибка парсинга списка пользователей");
        return;
    }
    QJsonArray arr = doc.array();
    QList<UserData> users;
    for (const auto& val : arr) {
        QJsonObject obj = val.toObject();
        UserData u;
        u.id = obj["id"].toInt();
        u.login = obj["login"].toString();
        u.fio = obj["fio"].toString();
        u.role = obj["role"].toString();
        users.append(u);
    }
    emit usersLoaded(users);
}

void ClientService::handleSuppliersResponse(const QString& response)
{
    QJsonDocument doc = parseJsonPayload(response, "OK SUPPLIERS ");
    if (doc.isNull() || !doc.isArray()) {
        emit operationError(0, "Ошибка парсинга списка поставщиков");
        return;
    }
    QJsonArray arr = doc.array();
    QList<SupplierData> suppliers;
    for (const auto& val : arr) {
        QJsonObject obj = val.toObject();
        SupplierData s;
        s.id = obj["id"].toInt();
        s.name = obj["name"].toString();
        s.phone = obj["phone"].toString();
        s.address = obj["address"].toString();
        suppliers.append(s);
    }
    emit suppliersLoaded(suppliers);
}

void ClientService::handleDetailsResponse(const QString& response)
{
    QJsonDocument doc = parseJsonPayload(response, "OK DETAILS ");
    if (doc.isNull() || !doc.isArray()) {
        emit operationError(0, "Ошибка парсинга списка деталей");
        return;
    }
    QJsonArray arr = doc.array();
    QList<DetailData> details;
    for (const auto& val : arr) {
        QJsonObject obj = val.toObject();
        DetailData d;
        d.id = obj["id"].toInt();
        d.article = obj["article"].toString();
        d.name = obj["name"].toString();
        details.append(d);
    }
    emit detailsLoaded(details);
}

void ClientService::handlePriceChangesResponse(const QString& response)
{
    QJsonDocument doc = parseJsonPayload(response, "OK PRICECHANGES ");
    if (doc.isNull() || !doc.isArray()) {
        emit operationError(0, "Ошибка парсинга списка изменений цен");
        return;
    }
    QJsonArray arr = doc.array();
    QList<PriceChangeData> changes;
    for (const auto& val : arr) {
        QJsonObject obj = val.toObject();
        PriceChangeData pc;
        pc.id = obj["id"].toInt();
        pc.detailId = obj["detail_id"].toInt();
        pc.supplierId = obj["supplier_id"].toInt();
        pc.changeDate = QDate::fromString(obj["change_date"].toString(), Qt::ISODate);
        pc.price = obj["price"].toDouble();
        pc.article = obj["article"].toString();
        pc.detailName = obj["detail_name"].toString();
        pc.supplierName = obj["supplier_name"].toString();
        changes.append(pc);
    }
    emit priceChangesLoaded(changes);
}

void ClientService::handleSuppliesResponse(const QString& response)
{
    QJsonDocument doc = parseJsonPayload(response, "OK SUPPLIES ");
    if (doc.isNull() || !doc.isArray()) {
        emit operationError(0, "Ошибка парсинга списка поставок");
        return;
    }
    QJsonArray arr = doc.array();
    QList<SupplyData> supplies;
    for (const auto& val : arr) {
        QJsonObject obj = val.toObject();
        SupplyData s;
        s.supplyId = obj["supply_id"].toInt();
        s.supplyDate = QDate::fromString(obj["supply_date"].toString(), Qt::ISODate);
        s.supplierName = obj["supplier_name"].toString();
        s.article = obj["article"].toString();
        s.detailName = obj["detail_name"].toString();
        s.price = obj["price"].toDouble();
        s.quantity = obj["quantity"].toInt();
        s.totalAmount = obj["total_amount"].toDouble();
        supplies.append(s);
    }
    emit suppliesLoaded(supplies);
}

void ClientService::handleCurrentPricesResponse(const QString& response)
{
    QJsonDocument doc = parseJsonPayload(response, "OK CURRENTPRICES ");
    if (doc.isNull() || !doc.isArray()) {
        emit operationError(0, "Ошибка парсинга текущих цен");
        return;
    }
    QJsonArray arr = doc.array();
    QList<CurrentPriceData> prices;
    for (const auto& val : arr) {
        QJsonObject obj = val.toObject();
        CurrentPriceData cp;
        cp.detailId = obj["detail_id"].toInt();
        cp.article = obj["article"].toString();
        cp.detailName = obj["detail_name"].toString();
        cp.price = obj["price"].toDouble();
        cp.changeDate = QDate::fromString(obj["change_date"].toString(), Qt::ISODate);
        cp.supplierId = obj["supplier_id"].toInt();
        cp.supplierName = obj["supplier_name"].toString();
        prices.append(cp);
    }
    emit currentPricesLoaded(prices);
}

void ClientService::handlePriceHistoryResponse(const QString& response)
{
    QJsonDocument doc = parseJsonPayload(response, "OK PRICEHISTORY ");
    if (doc.isNull() || !doc.isArray()) {
        emit operationError(0, "Ошибка парсинга истории цен");
        return;
    }
    QJsonArray arr = doc.array();
    QList<PriceHistoryData> history;
    for (const auto& val : arr) {
        QJsonObject obj = val.toObject();
        PriceHistoryData ph;
        ph.article = obj["article"].toString();
        ph.detailName = obj["detail_name"].toString();
        ph.supplierName = obj["supplier_name"].toString();
        ph.changeDate = QDate::fromString(obj["change_date"].toString(), Qt::ISODate);
        ph.price = obj["price"].toDouble();
        history.append(ph);
    }
    emit priceHistoryLoaded(history);
}

void ClientService::handleAccountingResponse(const QString& response)
{
    QJsonDocument doc = parseJsonPayload(response, "OK ACCOUNTING ");
    if (doc.isNull() || !doc.isArray()) {
        emit operationError(0, "Ошибка парсинга бухгалтерского отчёта");
        return;
    }
    QJsonArray arr = doc.array();
    QList<AccountingData> accounting;
    for (const auto& val : arr) {
        QJsonObject obj = val.toObject();
        AccountingData a;
        a.supplyDate = QDate::fromString(obj["supply_date"].toString(), Qt::ISODate);
        a.supplierName = obj["supplier_name"].toString();
        a.article = obj["article"].toString();
        a.detailName = obj["detail_name"].toString();
        a.quantity = obj["quantity"].toInt();
        a.unitPrice = obj["unit_price"].toDouble();
        a.totalAmount = obj["total_amount"].toDouble();
        accounting.append(a);
    }
    emit accountingLoaded(accounting);
}

void ClientService::handleOperationResponse(const QString& response)
{
    if (response.startsWith("OK ")) {
        emit operationSuccess(0, response.mid(3));
    } else {
        QString err = response.startsWith("ERROR ") ? response.mid(6) : "Неизвестная ошибка";
        emit operationError(0, err);
    }
}