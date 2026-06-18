#include "../Model/supplier.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "userdialog.h"
#include "supplierdialog.h"
#include "../Utils/sessionmanager.h"
#include "../Controller/usercontroller.h"
#include "../Model/userinfoview.h"
#include "../Controller/suppliercontroller.h"
#include <QApplication>
#include <QMessageBox>
#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Показываем диалог входа
    LoginDialog loginDlg(this);
    if (loginDlg.exec() != QDialog::Accepted) {
        QApplication::quit();
        return;
    }

    // После успешного входа настраиваем интерфейс под роль
    User *user = SessionManager::instance()->currentUser();
    if (!user) {
        QMessageBox::critical(this, "Ошибка", "Не удалось получить данные пользователя.");
        QApplication::quit();
        return;
    }

    ui->statusbar->showMessage(
        QString("Авторизован: %1 (%2)").arg(user->fio(), user->role()));

    setupUiForRole(user->role());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUiForRole(const QString &role)
{
    // Очищаем вкладки (на случай, если они уже есть)
    ui->tabWidgetMain->clear();

    if (role == "admin") {
        addAdminTabs();
        // Администратор видит всё
        addManagerTabs();
        addAccountantTabs();
    } else if (role == "manager") {
        addManagerTabs();
    } else if (role == "accountant") {
        addAccountantTabs();
    }
}

void MainWindow::addAdminTabs()
{
    // --- Вкладка "Пользователи" ---
    QWidget *userTab = new QWidget();
    QVBoxLayout *userLayout = new QVBoxLayout(userTab);

    // Таблица
    m_usersTable = new QTableWidget(this);
    m_usersTable->setColumnCount(4);
    m_usersTable->setHorizontalHeaderLabels({"ID", "Логин", "ФИО", "Роль"});
    m_usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_usersTable->horizontalHeader()->setStretchLastSection(true);
    userLayout->addWidget(m_usersTable);

    // Панель кнопок
    QWidget *btnPanel = new QWidget();
    QHBoxLayout *btnLayout = new QHBoxLayout(btnPanel);
    QPushButton *btnAdd = new QPushButton("Добавить");
    QPushButton *btnRole = new QPushButton("Сменить роль");
    QPushButton *btnDelete = new QPushButton("Удалить");
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnRole);
    btnLayout->addWidget(btnDelete);
    btnLayout->addStretch();
    userLayout->addWidget(btnPanel);

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddUser);
    connect(btnRole, &QPushButton::clicked, this, &MainWindow::onChangeRole);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteUser);

    ui->tabWidgetMain->addTab(userTab, "Пользователи");

    // Загружаем данные
    refreshUsersTable();

    // Остальные вкладки (заглушки)
    QWidget *logTab = new QWidget();
    QLabel *logLabel = new QLabel("Журналы операций (будут реализованы)", logTab);
    logLabel->setAlignment(Qt::AlignCenter);
    ui->tabWidgetMain->addTab(logTab, "Журналы");
}

void MainWindow::refreshUsersTable()
{
    QList<UserInfoView*> users = UserController::getAllUsers();
    m_usersTable->setRowCount(users.size());
    for (int i = 0; i < users.size(); ++i) {
        UserInfoView *u = users[i];
        m_usersTable->setItem(i, 0, new QTableWidgetItem(QString::number(u->userId())));
        m_usersTable->setItem(i, 1, new QTableWidgetItem(u->login()));
        m_usersTable->setItem(i, 2, new QTableWidgetItem(u->fio()));
        m_usersTable->setItem(i, 3, new QTableWidgetItem(u->role()));
    }
    m_usersTable->resizeColumnsToContents();
    qDeleteAll(users); // чистим память
}

void MainWindow::onAddUser()
{
    UserDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        bool ok = UserController::createUser(dlg.login(), dlg.fio(), dlg.role());
        if (ok) {
            QMessageBox::information(this, "Успех", "Пользователь создан.");
            refreshUsersTable();
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось создать пользователя.");
        }
    }
}

void MainWindow::onChangeRole()
{
    int row = m_usersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите пользователя!");
        return;
    }
    int userId = m_usersTable->item(row, 0)->text().toInt();
    QString currentRole = m_usersTable->item(row, 3)->text();

    QStringList roles = {"admin", "manager", "accountant"};
    bool ok;
    QString newRole = QInputDialog::getItem(this, "Смена роли",
                                            "Выберите новую роль для пользователя:",
                                            roles, roles.indexOf(currentRole), false, &ok);
    if (ok && !newRole.isEmpty()) {
        if (UserController::changeRole(userId, newRole)) {
            QMessageBox::information(this, "Успех", "Роль изменена.");
            refreshUsersTable();
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось изменить роль.");
        }
    }
}

void MainWindow::onDeleteUser()
{
    int row = m_usersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите пользователя!");
        return;
    }
    int userId = m_usersTable->item(row, 0)->text().toInt();
    QString login = m_usersTable->item(row, 1)->text();

    if (QMessageBox::question(this, "Подтверждение",
                              QString("Удалить пользователя '%1'?").arg(login))
        == QMessageBox::Yes)
    {
        if (UserController::deleteUser(userId)) {
            QMessageBox::information(this, "Успех", "Пользователь удалён.");
            refreshUsersTable();
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить пользователя.");
        }
    }
}

void MainWindow::addManagerTabs()
{
    QWidget *suppliesTab = new QWidget();
    QLabel *suppliesLabel = new QLabel("Поставки (будет реализовано)", suppliesTab);
    suppliesLabel->setAlignment(Qt::AlignCenter);
    ui->tabWidgetMain->addTab(suppliesTab, "Поставки");

    QWidget *pricesTab = new QWidget();
    QLabel *pricesLabel = new QLabel("История цен (будет реализовано)", pricesTab);
    pricesLabel->setAlignment(Qt::AlignCenter);
    ui->tabWidgetMain->addTab(pricesTab, "Цены");

    // Вкладка "Поставщики"
    QWidget *suppliersTab = new QWidget();
    QVBoxLayout *suppliersLayout = new QVBoxLayout(suppliersTab);

    m_suppliersTable = new QTableWidget(this);
    m_suppliersTable->setColumnCount(4);
    m_suppliersTable->setHorizontalHeaderLabels({"ID", "Название", "Телефон", "Адрес"});
    m_suppliersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_suppliersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_suppliersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_suppliersTable->horizontalHeader()->setStretchLastSection(true);
    suppliersLayout->addWidget(m_suppliersTable);

    // Панель кнопок
    QWidget *btnPanel = new QWidget();
    QHBoxLayout *btnLayout = new QHBoxLayout(btnPanel);
    QPushButton *btnAdd = new QPushButton("Добавить");
    QPushButton *btnEdit = new QPushButton("Редактировать");
    QPushButton *btnDelete = new QPushButton("Удалить");
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDelete);
    btnLayout->addStretch();
    suppliersLayout->addWidget(btnPanel);

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddSupplier);
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::onEditSupplier);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteSupplier);

    ui->tabWidgetMain->addTab(suppliersTab, "Поставщики");

    refreshSuppliersTable();
}

// Поставщики
void MainWindow::refreshSuppliersTable()
{
    QList<Supplier*> suppliers = SupplierController::getAllSuppliers();
    m_suppliersTable->setRowCount(suppliers.size());
    for (int i = 0; i < suppliers.size(); ++i) {
        Supplier *s = suppliers[i];
        m_suppliersTable->setItem(i, 0, new QTableWidgetItem(QString::number(s->supplierId())));
        m_suppliersTable->setItem(i, 1, new QTableWidgetItem(s->name()));
        m_suppliersTable->setItem(i, 2, new QTableWidgetItem(s->phone()));
        m_suppliersTable->setItem(i, 3, new QTableWidgetItem(s->address()));
    }
    m_suppliersTable->resizeColumnsToContents();
    qDeleteAll(suppliers);
}

void MainWindow::onAddSupplier()
{
    SupplierDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        bool ok = SupplierController::addSupplier(dlg.name(), dlg.phone(), dlg.address());
        if (ok) {
            QMessageBox::information(this, "Успех", "Поставщик добавлен.");
            refreshSuppliersTable();
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось добавить поставщика.");
        }
    }
}

void MainWindow::onEditSupplier()
{
    int row = m_suppliersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите поставщика!");
        return;
    }
    int id = m_suppliersTable->item(row, 0)->text().toInt();
    QString name = m_suppliersTable->item(row, 1)->text();
    QString phone = m_suppliersTable->item(row, 2)->text();
    QString address = m_suppliersTable->item(row, 3)->text();

    SupplierDialog dlg(this);
    dlg.setName(name);
    dlg.setPhone(phone);
    dlg.setAddress(address);
    if (dlg.exec() == QDialog::Accepted) {
        bool ok = SupplierController::updateSupplier(id, dlg.name(), dlg.phone(), dlg.address());
        if (ok) {
            QMessageBox::information(this, "Успех", "Поставщик обновлён.");
            refreshSuppliersTable();
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось обновить поставщика.");
        }
    }
}

void MainWindow::onDeleteSupplier()
{
    int row = m_suppliersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите поставщика!");
        return;
    }
    int id = m_suppliersTable->item(row, 0)->text().toInt();
    QString name = m_suppliersTable->item(row, 1)->text();

    if (QMessageBox::question(this, "Подтверждение",
                              QString("Удалить поставщика '%1'?").arg(name))
        == QMessageBox::Yes)
    {
        if (SupplierController::deleteSupplier(id)) {
            QMessageBox::information(this, "Успех", "Поставщик удалён.");
            refreshSuppliersTable();
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить поставщика (возможно, есть связанные поставки).");
        }
    }
}

void MainWindow::addAccountantTabs()
{
    // Вкладки для бухгалтера: отчёты
    QWidget *reportTab = new QWidget();
    QLabel *reportLabel = new QLabel("Финансовый отчёт (будет реализовано)", reportTab);
    reportLabel->setAlignment(Qt::AlignCenter);
    ui->tabWidgetMain->addTab(reportTab, "Отчёты");

    QWidget *priceHistTab = new QWidget();
    QLabel *priceHistLabel = new QLabel("История цен (будет реализовано)", priceHistTab);
    priceHistLabel->setAlignment(Qt::AlignCenter);
    ui->tabWidgetMain->addTab(priceHistTab, "Цены");
}