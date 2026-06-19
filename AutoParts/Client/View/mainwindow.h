#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QList>
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
    void refreshPriceHistoryTable();
    void refreshAccountingTable();

private:
    void setupUiForRole(const QString &role);
    void addAdminTabs();
    void addManagerTabs();
    void addAccountantTabs();
    void applyFilter(QTableWidget *table, const QString &text);

    Ui::MainWindow *ui;
    TcpClient *m_client;

    // Таблицы
    QTableWidget *m_usersTable = nullptr;
    QTableWidget *m_suppliersTable = nullptr;
    QTableWidget *m_detailsTable = nullptr;
    QTableWidget *m_priceChangesTable = nullptr;
    QTableWidget *m_suppliesTable = nullptr;
    QTableWidget *m_priceHistoryTable = nullptr;
    QTableWidget *m_accountingTable = nullptr;

    // Поля поиска
    QLineEdit *m_searchSuppliers = nullptr;
    QLineEdit *m_searchDetails = nullptr;
    QLineEdit *m_searchPriceChanges = nullptr;
    QLineEdit *m_searchSupplies = nullptr;

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
