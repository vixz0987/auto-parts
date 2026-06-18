#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "../Model/user.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // Построение интерфейса в зависимости от роли
    void setupUiForRole(const QString &role);

private:
    Ui::MainWindow *ui;

    // Вкладки для разных ролей
    void addAdminTabs();
    void addManagerTabs();
    void addAccountantTabs();

    // Вспомогательные методы для вкладки "Пользователи"
    void refreshUsersTable();
    void onAddUser();
    void onChangeRole();
    void onDeleteUser();

    QTableWidget *m_usersTable;  // таблица пользователей

    // Поставщики
    void refreshSuppliersTable();
    void onAddSupplier();
    void onEditSupplier();
    void onDeleteSupplier();
    QTableWidget *m_suppliersTable;
};

#endif // MAINWINDOW_H
