#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QTimer>
#include <QList>
#include <QPushButton>
#include "TcpClient/tcpclient.h"
#include "Utils/sessionmanager.h"
#include "Utils/datatypes.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(TcpClient *client, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onServerResponse(quint32 id, const QString &response);
    void onPeriodicRefresh();
    void addCommonTabs();

    void onProfile();
    void onLogout();

    // Пользователи
    void onAddUser();
    void onChangeRole();
    void onDeleteUser();
    void refreshUsersTable();

    // Поставщики
    void onAddSupplier();
    void onEditSupplier();
    void onDeleteSupplier();
    void refreshSuppliersTable();

    // Детали
    void onAddDetail();
    void onEditDetail();
    void onDeleteDetail();
    void refreshDetailsTable();

    // Изменения цен
    void onAddPriceChange();
    void onEditPriceChange();
    void onDeletePriceChange();
    void refreshPriceChangesTable();

    // Поставки
    void onAddSupply();
    void onEditSupply();
    void onDeleteSupply();
    void refreshSuppliesTable();

    // Представления
    void refreshAccountingTable();
    void refreshPriceHistoryTable();
    void refreshCurrentPricesTable();

private:
    void setupUiForRole(const QString &role);
    void addAdminTabs();
    void addManagerTabs();
    void addAccountantTabs();
    void applyFilter(QTableWidget *table, const QString &text);

    Ui::MainWindow *ui;
    TcpClient *m_client;

    QTimer *m_updateTimer;

    QTableWidget *m_priceHistoryTable = nullptr;
    QLineEdit *m_searchPriceHistory = nullptr;

    QTableWidget *m_currentPricesTable = nullptr;
    QLineEdit *m_searchCurrentPrices = nullptr;
    quint32 m_pendingCurrentPricesId = 0;

    QPushButton *m_profileButton = nullptr;
    QPushButton *m_logoutButton = nullptr;

    // Таблицы
    QTableWidget *m_usersTable = nullptr;
    QTableWidget *m_suppliersTable = nullptr;
    QTableWidget *m_detailsTable = nullptr;
    QTableWidget *m_priceChangesTable = nullptr;
    QTableWidget *m_suppliesTable = nullptr;
    QTableWidget *m_accountingTable = nullptr;

    // Поля поиска
    QLineEdit *m_searchSuppliers = nullptr;
    QLineEdit *m_searchDetails = nullptr;
    QLineEdit *m_searchPriceChanges = nullptr;
    QLineEdit *m_searchSupplies = nullptr;
    QLineEdit *m_accountingSearch = nullptr;

    // Кеш справочных данных
    QList<DetailItem> m_detailItems;
    QList<SupplierItem> m_supplierItems;
    QList<PriceChangeItem> m_priceChangeItems;   // для отображения в таблице и выбора в поставке

    // Идентификаторы запросов
    quint32 m_pendingUsersId = 0;
    quint32 m_pendingSuppliersId = 0;
    quint32 m_pendingDetailsId = 0;
    quint32 m_pendingPriceChangesId = 0;
    quint32 m_pendingSuppliesId = 0;
    quint32 m_pendingPriceHistoryId = 0;
    quint32 m_pendingAccountingId = 0;
    quint32 m_pendingGenericId = 0;
};

#endif // MAINWINDOW_H
