#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDate>
#include "TcpClient/tcpclient.h"

// Структуры данных для клиента
struct UserData {
    int id = 0;
    QString login;
    QString fio;
    QString role;
};

struct SupplierData {
    int id = 0;
    QString name;
    QString phone;
    QString address;
};

struct DetailData {
    int id = 0;
    QString article;
    QString name;
};

struct PriceChangeData {
    int id = 0;
    int detailId = 0;
    int supplierId = 0;
    QDate changeDate;
    double price = 0.0;
    QString article;
    QString detailName;
    QString supplierName;
};

struct SupplyData {
    int supplyId = 0;
    QDate supplyDate;
    QString supplierName;
    QString article;
    QString detailName;
    double price = 0.0;
    int quantity = 0;
    double totalAmount = 0.0;
};

struct AccountingData {
    QDate supplyDate;
    QString supplierName;
    QString article;
    QString detailName;
    int quantity = 0;
    double unitPrice = 0.0;
    double totalAmount = 0.0;
};

struct PriceHistoryData {
    QString article;
    QString detailName;
    QString supplierName;
    QDate changeDate;
    double price = 0.0;
};

struct CurrentPriceData {
    int detailId = 0;
    QString article;
    QString detailName;
    double price = 0.0;
    QDate changeDate;
    int supplierId = 0;
    QString supplierName;
};

class ClientService : public QObject
{
    Q_OBJECT
public:
    explicit ClientService(TcpClient* client, QObject* parent = nullptr);

    // ---------- Аутентификация ----------
    void login(const QString& login, const QString& password);
    void activate(const QString& login, const QString& fio, const QString& password);

    // ---------- Пользователи ----------
    void fetchUsers();
    void addUser(const QString& login, const QString& fio, const QString& role);
    void changeRole(int userId, const QString& newRole);
    void deleteUser(int userId);
    void changePassword(int userId, const QString& oldPassword, const QString& newPassword);
    void changeFio(int userId, const QString& newFio);

    // ---------- Поставщики ----------
    void fetchSuppliers();
    void addSupplier(const QString& name, const QString& phone, const QString& address);
    void updateSupplier(int id, const QString& name, const QString& phone, const QString& address);
    void deleteSupplier(int id);

    // ---------- Детали ----------
    void fetchDetails();
    void addDetail(const QString& article, const QString& name);
    void updateDetail(int id, const QString& article, const QString& name);
    void deleteDetail(int id);

    // ---------- Изменения цен ----------
    void fetchPriceChanges();
    void addPriceChange(int detailId, const QDate& changeDate, double price, int supplierId);
    void updatePriceChange(int id, int detailId, const QDate& changeDate, double price, int supplierId);
    void deletePriceChange(int id);

    // ---------- Поставки ----------
    void fetchSupplies();
    void addSupply(const QDate& supplyDate, int quantity, int priceChangeId);
    void updateSupply(int supplyId, const QDate& supplyDate, int quantity, int priceChangeId);
    void deleteSupply(int supplyId);

    // ---------- Представления ----------
    void fetchCurrentPrices();
    void fetchPriceHistory();
    void fetchAccounting();

signals:
    // Аутентификация
    void loginSuccess(const UserData& user);
    void loginError(const QString& error);
    void activationSuccess();
    void activationError(const QString& error);

    // Загрузка данных
    void usersLoaded(const QList<UserData>& users);
    void suppliersLoaded(const QList<SupplierData>& suppliers);
    void detailsLoaded(const QList<DetailData>& details);
    void priceChangesLoaded(const QList<PriceChangeData>& changes);
    void suppliesLoaded(const QList<SupplyData>& supplies);
    void currentPricesLoaded(const QList<CurrentPriceData>& prices);
    void priceHistoryLoaded(const QList<PriceHistoryData>& history);
    void accountingLoaded(const QList<AccountingData>& accounting);

    // Операции (добавление/обновление/удаление)
    void operationSuccess(quint32 requestId, const QString& message);
    void operationError(quint32 requestId, const QString& error);

private slots:
    void onResponse(quint32 id, const QString& response);

private:
    TcpClient* m_client;

    quint32 m_loginRequestId = 0;
    quint32 m_activationRequestId = 0;
    quint32 m_usersFetchId = 0;
    quint32 m_suppliersFetchId = 0;
    quint32 m_detailsFetchId = 0;
    quint32 m_priceChangesFetchId = 0;
    quint32 m_suppliesFetchId = 0;
    quint32 m_currentPricesFetchId = 0;
    quint32 m_priceHistoryFetchId = 0;
    quint32 m_accountingFetchId = 0;
    quint32 m_operationRequestId = 0;

    quint32 sendCommand(const QString& cmd);
    QJsonDocument parseJsonPayload(const QString& response, const QString& expectedPrefix);

    void handleLoginResponse(const QString& payload);
    void handleActivationResponse(const QString& payload);
    void handleUsersResponse(const QString& payload);
    void handleSuppliersResponse(const QString& payload);
    void handleDetailsResponse(const QString& payload);
    void handlePriceChangesResponse(const QString& payload);
    void handleSuppliesResponse(const QString& payload);
    void handleCurrentPricesResponse(const QString& payload);
    void handlePriceHistoryResponse(const QString& payload);
    void handleAccountingResponse(const QString& payload);
    void handleOperationResponse(const QString& payload);
};

#endif // CLIENTSERVICE_H