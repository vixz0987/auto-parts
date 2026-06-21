#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QTimer>
#include <QPushButton>
#include "TcpClient/tcpclient.h"
#include "Services/clientservice.h"
#include "Services/sessionmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(TcpClient* client, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onProfile();
    void onLogout();
    void onPeriodicRefresh();

    void onAddUser();
    void onChangeRole();
    void onDeleteUser();
    void refreshUsersTable();

    void onAddSupplier();
    void onEditSupplier();
    void onDeleteSupplier();
    void refreshSuppliersTable();

    void onAddDetail();
    void onEditDetail();
    void onDeleteDetail();
    void refreshDetailsTable();

    void onAddPriceChange();
    void onEditPriceChange();
    void onDeletePriceChange();
    void refreshPriceChangesTable();

    void onAddSupply();
    void onEditSupply();
    void onDeleteSupply();
    void refreshSuppliesTable();

    void refreshAccountingTable();
    void refreshPriceHistoryTable();
    void refreshCurrentPricesTable();

    void updateUsersTable(const QList<UserData>& users);
    void updateSuppliersTable(const QList<SupplierData>& suppliers);
    void updateDetailsTable(const QList<DetailData>& details);
    void updatePriceChangesTable(const QList<PriceChangeData>& changes);
    void updateSuppliesTable(const QList<SupplyData>& supplies);
    void updateCurrentPricesTable(const QList<CurrentPriceData>& prices);
    void updatePriceHistoryTable(const QList<PriceHistoryData>& history);
    void updateAccountingTable(const QList<AccountingData>& accounting);

    void onOperationSuccess(quint32 requestId, const QString& message);
    void onOperationError(quint32 requestId, const QString& error);

private:
    void setupUiForRole(const QString& role);
    void addCommonTabs();
    void addAdminTabs();
    void addManagerTabs();
    void addAccountantTabs();
    void applyFilter(QTableWidget* table, const QString& text);

    Ui::MainWindow* ui;
    TcpClient* m_client;
    ClientService* m_clientService;
    QTimer* m_updateTimer;

    QPushButton* m_profileButton = nullptr;
    QPushButton* m_logoutButton = nullptr;

    QTableWidget* m_usersTable = nullptr;
    QTableWidget* m_suppliersTable = nullptr;
    QTableWidget* m_detailsTable = nullptr;
    QTableWidget* m_priceChangesTable = nullptr;
    QTableWidget* m_suppliesTable = nullptr;
    QTableWidget* m_accountingTable = nullptr;
    QTableWidget* m_priceHistoryTable = nullptr;
    QTableWidget* m_currentPricesTable = nullptr;

    QLineEdit* m_searchSuppliers = nullptr;
    QLineEdit* m_searchDetails = nullptr;
    QLineEdit* m_searchPriceChanges = nullptr;
    QLineEdit* m_searchSupplies = nullptr;
    QLineEdit* m_accountingSearch = nullptr;
    QLineEdit* m_searchPriceHistory = nullptr;
    QLineEdit* m_searchCurrentPrices = nullptr;
};

#endif // MAINWINDOW_H