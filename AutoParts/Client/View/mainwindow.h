#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "TcpClient/tcpclient.h"
#include "Utils/sessionmanager.h"

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

    // Поставки и цены
    void refreshSuppliesTable();
    void refreshPriceHistoryTable();
    void refreshAccountingTable();

private:
    void setupUiForRole(const QString &role);
    void addAdminTabs();
    void addManagerTabs();
    void addAccountantTabs();

    Ui::MainWindow *ui;
    TcpClient *m_client;

    QTableWidget *m_usersTable = nullptr;
    QTableWidget *m_suppliersTable = nullptr;
    QTableWidget *m_suppliesTable = nullptr;
    QTableWidget *m_priceHistoryTable = nullptr;
    QTableWidget *m_accountingTable = nullptr;

    quint32 m_pendingUsersId = 0;
    quint32 m_pendingSuppliersId = 0;
    quint32 m_pendingSuppliesId = 0;
    quint32 m_pendingPriceHistoryId = 0;
    quint32 m_pendingAccountingId = 0;
    quint32 m_pendingGenericId = 0;
};

#endif // MAINWINDOW_H