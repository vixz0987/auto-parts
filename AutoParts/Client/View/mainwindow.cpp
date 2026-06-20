#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "userdialog.h"
#include "supplierdialog.h"
#include "detaildialog.h"
#include "pricechangedialog.h"
#include "supplydialog.h"
#include "profiledialog.h"
#include "Services/ClientService.h"
#include <QApplication>
#include <QMessageBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QInputDialog>
#include <QCheckBox>
#include <QScrollArea>
#include <QDebug>

MainWindow::MainWindow(TcpClient *client, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_client(client)
{
    ui->setupUi(this);

    UserData user = SessionManager::instance()->currentUser();
    if (user.id == 0) {
        QMessageBox::critical(this, "Error", "Нет авторизованного пользователя.");
        QApplication::quit();
        return;
    }

    m_clientService = new ClientService(m_client, this);

    connect(m_clientService, &ClientService::usersLoaded,
            this, &MainWindow::updateUsersTable);
    connect(m_clientService, &ClientService::suppliersLoaded,
            this, &MainWindow::updateSuppliersTable);
    connect(m_clientService, &ClientService::detailsLoaded,
            this, &MainWindow::updateDetailsTable);
    connect(m_clientService, &ClientService::priceChangesLoaded,
            this, &MainWindow::updatePriceChangesTable);
    connect(m_clientService, &ClientService::suppliesLoaded,
            this, &MainWindow::updateSuppliesTable);
    connect(m_clientService, &ClientService::currentPricesLoaded,
            this, &MainWindow::updateCurrentPricesTable);
    connect(m_clientService, &ClientService::priceHistoryLoaded,
            this, &MainWindow::updatePriceHistoryTable);
    connect(m_clientService, &ClientService::accountingLoaded,
            this, &MainWindow::updateAccountingTable);

    connect(m_clientService, &ClientService::operationSuccess,
            this, &MainWindow::onOperationSuccess);
    connect(m_clientService, &ClientService::operationError,
            this, &MainWindow::onOperationError);

    m_profileButton = new QPushButton("Профиль", this);
    m_logoutButton = new QPushButton("Выйти", this);
    m_profileButton->setFixedSize(80, 25);
    m_logoutButton->setFixedSize(80, 25);
    connect(m_profileButton, &QPushButton::clicked, this, &MainWindow::onProfile);
    connect(m_logoutButton, &QPushButton::clicked, this, &MainWindow::onLogout);
    ui->statusbar->addPermanentWidget(m_profileButton);
    ui->statusbar->addPermanentWidget(m_logoutButton);

    ui->statusbar->showMessage(QString("User: %1 (%2)").arg(user.fio, user.role));

    setupUiForRole(user.role);

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::onPeriodicRefresh);
    m_updateTimer->start(5000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onProfile()
{
    UserData user = SessionManager::instance()->currentUser();
    ProfileDialog dlg(m_clientService, user.id, this);
    if (dlg.exec() == QDialog::Accepted) {
        UserData updatedUser = SessionManager::instance()->currentUser();
        ui->statusbar->showMessage(QString("User: %1 (%2)").arg(updatedUser.fio, updatedUser.role));
    }
}

void MainWindow::onLogout()
{
    SessionManager::instance()->logout();
    this->close();
    QApplication::quit();
}

// ----------------------------------------------------------------
// Настройка вкладок
// ----------------------------------------------------------------

void MainWindow::addCommonTabs()
{
    QWidget *priceTab = new QWidget();
    QVBoxLayout *priceLayout = new QVBoxLayout(priceTab);

    m_currentPricesTable = new QTableWidget(this);
    m_currentPricesTable->setColumnCount(6);
    m_currentPricesTable->setHorizontalHeaderLabels({"Артикул", "Деталь", "Цена", "Дата изменения", "Поставщик", "ID поставщика"});
    m_currentPricesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_currentPricesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_currentPricesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_currentPricesTable->setWordWrap(true);
    m_currentPricesTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    priceLayout->addWidget(m_currentPricesTable);

    m_searchCurrentPrices = new QLineEdit();
    m_searchCurrentPrices->setPlaceholderText("Поиск по текущим ценам...");
    priceLayout->insertWidget(0, m_searchCurrentPrices);
    connect(m_searchCurrentPrices, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_currentPricesTable, text);
    });

    ui->tabWidgetMain->addTab(priceTab, "Текущие цены");
    refreshCurrentPricesTable();
}

void MainWindow::setupUiForRole(const QString &role)
{
    ui->tabWidgetMain->clear();
    addCommonTabs();

    if (role == "admin") {
        addAdminTabs();
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
    QWidget *userTab = new QWidget();
    QVBoxLayout *userLayout = new QVBoxLayout(userTab);

    m_usersTable = new QTableWidget(this);
    m_usersTable->setColumnCount(4);
    m_usersTable->setHorizontalHeaderLabels({"ID", "Логин", "ФИО", "Роль"});
    m_usersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_usersTable->setWordWrap(true);
    m_usersTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    userLayout->addWidget(m_usersTable);

    QWidget *btnPanel = new QWidget();
    QHBoxLayout *btnLayout = new QHBoxLayout(btnPanel);
    QPushButton *btnAdd = new QPushButton("Добавить");
    QPushButton *btnRole = new QPushButton("Изменить роль");
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
    refreshUsersTable();
}

void MainWindow::addManagerTabs()
{
    // ДЕТАЛИ
    QWidget *detailsTab = new QWidget();
    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsTab);

    m_detailsTable = new QTableWidget(this);
    m_detailsTable->setColumnCount(3);
    m_detailsTable->setHorizontalHeaderLabels({"ID", "Артикул", "Название"});
    m_detailsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_detailsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_detailsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_detailsTable->setWordWrap(true);
    m_detailsTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    detailsLayout->addWidget(m_detailsTable);

    m_searchDetails = new QLineEdit();
    m_searchDetails->setPlaceholderText("Поиск деталей...");
    detailsLayout->insertWidget(0, m_searchDetails);
    connect(m_searchDetails, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_detailsTable, text);
    });

    QWidget *detailsBtns = new QWidget();
    QHBoxLayout *dlBtns = new QHBoxLayout(detailsBtns);
    QPushButton *btnAddDetail = new QPushButton("Добавить");
    QPushButton *btnEditDetail = new QPushButton("Изменить");
    QPushButton *btnDelDetail = new QPushButton("Удалить");
    dlBtns->addWidget(btnAddDetail);
    dlBtns->addWidget(btnEditDetail);
    dlBtns->addWidget(btnDelDetail);
    detailsLayout->addWidget(detailsBtns);

    connect(btnAddDetail, &QPushButton::clicked, this, &MainWindow::onAddDetail);
    connect(btnEditDetail, &QPushButton::clicked, this, &MainWindow::onEditDetail);
    connect(btnDelDetail, &QPushButton::clicked, this, &MainWindow::onDeleteDetail);

    ui->tabWidgetMain->addTab(detailsTab, "Детали");
    refreshDetailsTable();

    // ЦЕНОВЫЕ ИЗМЕНЕНИЯ
    QWidget *pcTab = new QWidget();
    QVBoxLayout *pcLayout = new QVBoxLayout(pcTab);

    m_priceChangesTable = new QTableWidget(this);
    m_priceChangesTable->setColumnCount(6);
    m_priceChangesTable->setHorizontalHeaderLabels({"ID", "Артикул", "Деталь", "Поставщик", "Дата изм.", "Цена"});
    m_priceChangesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_priceChangesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_priceChangesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_priceChangesTable->setWordWrap(true);
    m_priceChangesTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    pcLayout->addWidget(m_priceChangesTable);

    m_searchPriceChanges = new QLineEdit();
    m_searchPriceChanges->setPlaceholderText("Поиск изменения...");
    pcLayout->insertWidget(0, m_searchPriceChanges);
    connect(m_searchPriceChanges, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_priceChangesTable, text);
    });

    QWidget *pcBtns = new QWidget();
    QHBoxLayout *pcBL = new QHBoxLayout(pcBtns);
    QPushButton *btnAddPC = new QPushButton("Добавить");
    QPushButton *btnEditPC = new QPushButton("Изменить");
    QPushButton *btnDelPC = new QPushButton("Удалить");
    pcBL->addWidget(btnAddPC);
    pcBL->addWidget(btnEditPC);
    pcBL->addWidget(btnDelPC);
    pcLayout->addWidget(pcBtns);

    connect(btnAddPC, &QPushButton::clicked, this, &MainWindow::onAddPriceChange);
    connect(btnEditPC, &QPushButton::clicked, this, &MainWindow::onEditPriceChange);
    connect(btnDelPC, &QPushButton::clicked, this, &MainWindow::onDeletePriceChange);

    ui->tabWidgetMain->addTab(pcTab, "Ценовые изменения");
    refreshPriceChangesTable();

    // ПОСТАВКИ
    QWidget *supTab = new QWidget();
    QVBoxLayout *supLayout = new QVBoxLayout(supTab);

    m_suppliesTable = new QTableWidget(this);
    m_suppliesTable->setColumnCount(8);
    m_suppliesTable->setHorizontalHeaderLabels({"ID", "Дата поставки", "Поставщик", "Артикул", "Деталь", "Цена", "Кол-во", "Сумма"});
    m_suppliesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_suppliesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_suppliesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_suppliesTable->setWordWrap(true);
    m_suppliesTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    supLayout->addWidget(m_suppliesTable);

    m_searchSupplies = new QLineEdit();
    m_searchSupplies->setPlaceholderText("Поиск поставок...");
    supLayout->insertWidget(0, m_searchSupplies);
    connect(m_searchSupplies, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_suppliesTable, text);
    });

    QWidget *supBtns = new QWidget();
    QHBoxLayout *supBL = new QHBoxLayout(supBtns);
    QPushButton *btnAddSup = new QPushButton("Добавить");
    QPushButton *btnEditSup = new QPushButton("Изменить");
    QPushButton *btnDelSup = new QPushButton("Удалить");
    supBL->addWidget(btnAddSup);
    supBL->addWidget(btnEditSup);
    supBL->addWidget(btnDelSup);
    supLayout->addWidget(supBtns);

    connect(btnAddSup, &QPushButton::clicked, this, &MainWindow::onAddSupply);
    connect(btnEditSup, &QPushButton::clicked, this, &MainWindow::onEditSupply);
    connect(btnDelSup, &QPushButton::clicked, this, &MainWindow::onDeleteSupply);

    ui->tabWidgetMain->addTab(supTab, "Поставки");
    refreshSuppliesTable();

    // ПОСТАВЩИКИ
    QWidget *suppliersTab = new QWidget();
    QVBoxLayout *suppliersLayout = new QVBoxLayout(suppliersTab);

    m_suppliersTable = new QTableWidget(this);
    m_suppliersTable->setColumnCount(4);
    m_suppliersTable->setHorizontalHeaderLabels({"ID", "Название", "Номер тел.", "Адрес"});
    m_suppliersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_suppliersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_suppliersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_suppliersTable->setWordWrap(true);
    m_suppliersTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    suppliersLayout->addWidget(m_suppliersTable);

    m_searchSuppliers = new QLineEdit();
    m_searchSuppliers->setPlaceholderText("Поиск поставщиков...");
    suppliersLayout->insertWidget(0, m_searchSuppliers);
    connect(m_searchSuppliers, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_suppliersTable, text);
    });

    QWidget *btnPanel = new QWidget();
    QHBoxLayout *btnLayout = new QHBoxLayout(btnPanel);
    QPushButton *btnAdd = new QPushButton("Добавить");
    QPushButton *btnEdit = new QPushButton("Изменить");
    QPushButton *btnDelete = new QPushButton("Удалить");
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDelete);
    suppliersLayout->addWidget(btnPanel);

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddSupplier);
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::onEditSupplier);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteSupplier);

    ui->tabWidgetMain->addTab(suppliersTab, "Поставщики");
    refreshSuppliersTable();
}

void MainWindow::addAccountantTabs()
{
    // Бухгалтерский отчёт
    QWidget *accTab = new QWidget();
    QVBoxLayout *accLayout = new QVBoxLayout(accTab);

    m_accountingSearch = new QLineEdit();
    m_accountingSearch->setPlaceholderText("Поиск...");
    accLayout->addWidget(m_accountingSearch);

    m_accountingTable = new QTableWidget(this);
    m_accountingTable->setColumnCount(7);
    m_accountingTable->setHorizontalHeaderLabels({"Дата поставки", "Поставщик", "Артикул", "Деталь", "Кол-во", "Цена", "Сумма"});
    m_accountingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_accountingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_accountingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_accountingTable->setWordWrap(true);
    m_accountingTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    accLayout->addWidget(m_accountingTable);

    connect(m_accountingSearch, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_accountingTable, text);
    });

    ui->tabWidgetMain->addTab(accTab, "Бухгалтерский отчет");
    refreshAccountingTable();

    // История цен
    QWidget *priceHistoryTab = new QWidget();
    QVBoxLayout *phLayout = new QVBoxLayout(priceHistoryTab);

    m_priceHistoryTable = new QTableWidget(this);
    m_priceHistoryTable->setColumnCount(5);
    m_priceHistoryTable->setHorizontalHeaderLabels({"Артикул", "Деталь", "Поставщик", "Дата изменения", "Цена"});
    m_priceHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_priceHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_priceHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_priceHistoryTable->setWordWrap(true);
    m_priceHistoryTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    phLayout->addWidget(m_priceHistoryTable);

    m_searchPriceHistory = new QLineEdit();
    m_searchPriceHistory->setPlaceholderText("Поиск в истории изменения цен...");
    phLayout->insertWidget(0, m_searchPriceHistory);
    connect(m_searchPriceHistory, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_priceHistoryTable, text);
    });

    ui->tabWidgetMain->addTab(priceHistoryTab, "История изменения цен");
    refreshPriceHistoryTable();
}

// ----------------------------------------------------------------
// Вспомогательные
// ----------------------------------------------------------------

void MainWindow::applyFilter(QTableWidget *table, const QString &text)
{
    for (int row = 0; row < table->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < table->columnCount(); ++col) {
            QTableWidgetItem *item = table->item(row, col);
            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        table->setRowHidden(row, !match);
    }
}

// ----------------------------------------------------------------
// CRUD: Детали
// ----------------------------------------------------------------

void MainWindow::onAddDetail()
{
    DetailDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_clientService->addDetail(dlg.article(), dlg.name());
    }
}

void MainWindow::onEditDetail()
{
    int row = m_detailsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Выберите деталь!");
        return;
    }
    int id = m_detailsTable->item(row, 0)->text().toInt();
    DetailDialog dlg(this);
    dlg.setArticle(m_detailsTable->item(row, 1)->text());
    dlg.setName(m_detailsTable->item(row, 2)->text());
    if (dlg.exec() == QDialog::Accepted) {
        m_clientService->updateDetail(id, dlg.article(), dlg.name());
    }
}

void MainWindow::onDeleteDetail()
{
    int row = m_detailsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Выберите деталь!");
        return;
    }
    int id = m_detailsTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтвердить", "Удалить деталь?") == QMessageBox::Yes) {
        m_clientService->deleteDetail(id);
    }
}

void MainWindow::refreshDetailsTable()
{
    m_clientService->fetchDetails();
}

// ----------------------------------------------------------------
// CRUD: Изменения цен
// ----------------------------------------------------------------

void MainWindow::onAddPriceChange()
{
    PriceChangeDialog dlg(m_clientService, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_clientService->addPriceChange(dlg.selectedDetailId(), dlg.changeDate(),
                                        dlg.price(), dlg.selectedSupplierId());
    }
}

void MainWindow::onEditPriceChange()
{
    int row = m_priceChangesTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Выберите изменение цены!");
        return;
    }
    QMessageBox::information(this, "Редактирование", "Редактирование изменения цены требует дополнительной логики. Воспользуйтесь удалением и созданием заново.");
}

void MainWindow::onDeletePriceChange()
{
    int row = m_priceChangesTable->currentRow();
    if (row < 0) return;
    int id = m_priceChangesTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтвердить", "Удалить изменение цены?") == QMessageBox::Yes) {
        m_clientService->deletePriceChange(id);
    }
}

void MainWindow::refreshPriceChangesTable()
{
    m_clientService->fetchPriceChanges();
}

// ----------------------------------------------------------------
// CRUD: Поставки
// ----------------------------------------------------------------

void MainWindow::onAddSupply()
{
    SupplyDialog dlg(m_clientService, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_clientService->addSupply(dlg.supplyDate(), dlg.quantity(),
                                   dlg.selectedPriceChangeId());
    }
}

void MainWindow::onEditSupply()
{
    int row = m_suppliesTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Выберите поставку!");
        return;
    }
    QMessageBox::information(this, "Не реализовано", "Редактирование поставок реализовано не полностью. Удалите и создайте заново.");
}

void MainWindow::onDeleteSupply()
{
    int row = m_suppliesTable->currentRow();
    if (row < 0) return;
    int id = m_suppliesTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтвердить", "Удалить поставку?") == QMessageBox::Yes) {
        m_clientService->deleteSupply(id);
    }
}

void MainWindow::refreshSuppliesTable()
{
    m_clientService->fetchSupplies();
}

// ----------------------------------------------------------------
// CRUD: Пользователи
// ----------------------------------------------------------------

void MainWindow::onAddUser()
{
    UserDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_clientService->addUser(dlg.login(), dlg.fio(), dlg.role());
    }
}

void MainWindow::onChangeRole()
{
    int row = m_usersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Выберите пользователя!");
        return;
    }
    int userId = m_usersTable->item(row, 0)->text().toInt();
    QString currentRole = m_usersTable->item(row, 3)->text();
    QStringList roles = {"admin", "manager", "accountant"};
    bool ok;
    QString newRole = QInputDialog::getItem(this, "Изменить роль", "Выберите роль:", roles, roles.indexOf(currentRole), false, &ok);
    if (ok && !newRole.isEmpty()) {
        m_clientService->changeRole(userId, newRole);
    }
}

void MainWindow::onDeleteUser()
{
    int row = m_usersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Выберите пользователя!");
        return;
    }
    int userId = m_usersTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтвердить", "Удалить пользователя?") == QMessageBox::Yes) {
        m_clientService->deleteUser(userId);
    }
}

void MainWindow::refreshUsersTable()
{
    m_clientService->fetchUsers();
}

// ----------------------------------------------------------------
// CRUD: Поставщики
// ----------------------------------------------------------------

void MainWindow::onAddSupplier()
{
    SupplierDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_clientService->addSupplier(dlg.name(), dlg.phone(), dlg.address());
    }
}

void MainWindow::onEditSupplier()
{
    int row = m_suppliersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Выберите поставщика!");
        return;
    }
    int id = m_suppliersTable->item(row, 0)->text().toInt();
    SupplierDialog dlg(this);
    dlg.setName(m_suppliersTable->item(row, 1)->text());
    dlg.setPhone(m_suppliersTable->item(row, 2)->text());
    dlg.setAddress(m_suppliersTable->item(row, 3)->text());
    if (dlg.exec() == QDialog::Accepted) {
        m_clientService->updateSupplier(id, dlg.name(), dlg.phone(), dlg.address());
    }
}

void MainWindow::onDeleteSupplier()
{
    int row = m_suppliersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Выберите поставщика!");
        return;
    }
    int id = m_suppliersTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтвердить", "Удалить поставщика?") == QMessageBox::Yes) {
        m_clientService->deleteSupplier(id);
    }
}

void MainWindow::refreshSuppliersTable()
{
    m_clientService->fetchSuppliers();
}

// ----------------------------------------------------------------
// Представления
// ----------------------------------------------------------------

void MainWindow::refreshAccountingTable()
{
    m_clientService->fetchAccounting();
}

void MainWindow::refreshPriceHistoryTable()
{
    m_clientService->fetchPriceHistory();
}

void MainWindow::refreshCurrentPricesTable()
{
    m_clientService->fetchCurrentPrices();
}

// ----------------------------------------------------------------
// Слоты обновления таблиц из сервиса
// ----------------------------------------------------------------

void MainWindow::updateUsersTable(const QList<UserData>& users)
{
    if (!m_usersTable) return;
    m_usersTable->setRowCount(users.size());
    for (int i = 0; i < users.size(); ++i) {
        const UserData& u = users[i];
        m_usersTable->setItem(i, 0, new QTableWidgetItem(QString::number(u.id)));
        m_usersTable->setItem(i, 1, new QTableWidgetItem(u.login));
        m_usersTable->setItem(i, 2, new QTableWidgetItem(u.fio));
        m_usersTable->setItem(i, 3, new QTableWidgetItem(u.role));
    }
    m_usersTable->resizeRowsToContents();
}

void MainWindow::updateSuppliersTable(const QList<SupplierData>& suppliers)
{
    if (!m_suppliersTable) return;
    m_suppliersTable->setRowCount(suppliers.size());
    for (int i = 0; i < suppliers.size(); ++i) {
        const SupplierData& s = suppliers[i];
        m_suppliersTable->setItem(i, 0, new QTableWidgetItem(QString::number(s.id)));
        m_suppliersTable->setItem(i, 1, new QTableWidgetItem(s.name));
        m_suppliersTable->setItem(i, 2, new QTableWidgetItem(s.phone));
        m_suppliersTable->setItem(i, 3, new QTableWidgetItem(s.address));
    }
    m_suppliersTable->resizeRowsToContents();
}

void MainWindow::updateDetailsTable(const QList<DetailData>& details)
{
    if (!m_detailsTable) return;
    m_detailsTable->setRowCount(details.size());
    for (int i = 0; i < details.size(); ++i) {
        const DetailData& d = details[i];
        m_detailsTable->setItem(i, 0, new QTableWidgetItem(QString::number(d.id)));
        m_detailsTable->setItem(i, 1, new QTableWidgetItem(d.article));
        m_detailsTable->setItem(i, 2, new QTableWidgetItem(d.name));
    }
    m_detailsTable->resizeRowsToContents();
}

void MainWindow::updatePriceChangesTable(const QList<PriceChangeData>& changes)
{
    if (!m_priceChangesTable) return;
    m_priceChangesTable->setRowCount(changes.size());
    for (int i = 0; i < changes.size(); ++i) {
        const PriceChangeData& pc = changes[i];
        m_priceChangesTable->setItem(i, 0, new QTableWidgetItem(QString::number(pc.id)));
        m_priceChangesTable->setItem(i, 1, new QTableWidgetItem(pc.article));
        m_priceChangesTable->setItem(i, 2, new QTableWidgetItem(pc.detailName));
        m_priceChangesTable->setItem(i, 3, new QTableWidgetItem(pc.supplierName));
        m_priceChangesTable->setItem(i, 4, new QTableWidgetItem(pc.changeDate.toString(Qt::ISODate)));
        m_priceChangesTable->setItem(i, 5, new QTableWidgetItem(QString::number(pc.price, 'f', 2)));
    }
    m_priceChangesTable->resizeRowsToContents();
}

void MainWindow::updateSuppliesTable(const QList<SupplyData>& supplies)
{
    if (!m_suppliesTable) return;
    m_suppliesTable->setRowCount(supplies.size());
    for (int i = 0; i < supplies.size(); ++i) {
        const SupplyData& s = supplies[i];
        m_suppliesTable->setItem(i, 0, new QTableWidgetItem(QString::number(s.supplyId)));
        m_suppliesTable->setItem(i, 1, new QTableWidgetItem(s.supplyDate.toString(Qt::ISODate)));
        m_suppliesTable->setItem(i, 2, new QTableWidgetItem(s.supplierName));
        m_suppliesTable->setItem(i, 3, new QTableWidgetItem(s.article));
        m_suppliesTable->setItem(i, 4, new QTableWidgetItem(s.detailName));
        m_suppliesTable->setItem(i, 5, new QTableWidgetItem(QString::number(s.price, 'f', 2)));
        m_suppliesTable->setItem(i, 6, new QTableWidgetItem(QString::number(s.quantity)));
        m_suppliesTable->setItem(i, 7, new QTableWidgetItem(QString::number(s.totalAmount, 'f', 2)));
    }
    m_suppliesTable->resizeRowsToContents();
}

void MainWindow::updateCurrentPricesTable(const QList<CurrentPriceData>& prices)
{
    if (!m_currentPricesTable) return;
    m_currentPricesTable->setRowCount(prices.size());
    for (int i = 0; i < prices.size(); ++i) {
        const CurrentPriceData& cp = prices[i];
        m_currentPricesTable->setItem(i, 0, new QTableWidgetItem(cp.article));
        m_currentPricesTable->setItem(i, 1, new QTableWidgetItem(cp.detailName));
        m_currentPricesTable->setItem(i, 2, new QTableWidgetItem(QString::number(cp.price, 'f', 2)));
        m_currentPricesTable->setItem(i, 3, new QTableWidgetItem(cp.changeDate.toString(Qt::ISODate)));
        m_currentPricesTable->setItem(i, 4, new QTableWidgetItem(cp.supplierName));
        m_currentPricesTable->setItem(i, 5, new QTableWidgetItem(QString::number(cp.supplierId)));
    }
    m_currentPricesTable->resizeRowsToContents();
}

void MainWindow::updatePriceHistoryTable(const QList<PriceHistoryData>& history)
{
    if (!m_priceHistoryTable) return;
    m_priceHistoryTable->setRowCount(history.size());
    for (int i = 0; i < history.size(); ++i) {
        const PriceHistoryData& ph = history[i];
        m_priceHistoryTable->setItem(i, 0, new QTableWidgetItem(ph.article));
        m_priceHistoryTable->setItem(i, 1, new QTableWidgetItem(ph.detailName));
        m_priceHistoryTable->setItem(i, 2, new QTableWidgetItem(ph.supplierName));
        m_priceHistoryTable->setItem(i, 3, new QTableWidgetItem(ph.changeDate.toString(Qt::ISODate)));
        m_priceHistoryTable->setItem(i, 4, new QTableWidgetItem(QString::number(ph.price, 'f', 2)));
    }
    m_priceHistoryTable->resizeRowsToContents();
}

void MainWindow::updateAccountingTable(const QList<AccountingData>& accounting)
{
    if (!m_accountingTable) return;
    m_accountingTable->setRowCount(accounting.size());
    for (int i = 0; i < accounting.size(); ++i) {
        const AccountingData& a = accounting[i];
        m_accountingTable->setItem(i, 0, new QTableWidgetItem(a.supplyDate.toString(Qt::ISODate)));
        m_accountingTable->setItem(i, 1, new QTableWidgetItem(a.supplierName));
        m_accountingTable->setItem(i, 2, new QTableWidgetItem(a.article));
        m_accountingTable->setItem(i, 3, new QTableWidgetItem(a.detailName));
        m_accountingTable->setItem(i, 4, new QTableWidgetItem(QString::number(a.quantity)));
        m_accountingTable->setItem(i, 5, new QTableWidgetItem(QString::number(a.unitPrice, 'f', 2)));
        m_accountingTable->setItem(i, 6, new QTableWidgetItem(QString::number(a.totalAmount, 'f', 2)));
    }
    m_accountingTable->resizeRowsToContents();
}

// ----------------------------------------------------------------
// Обработка результатов операций
// ----------------------------------------------------------------

void MainWindow::onOperationSuccess(quint32 /*requestId*/, const QString& message)
{
    QString friendlyMessage;
    if (message == "USERADDED") friendlyMessage = "Пользователь добавлен";
    else if (message == "ROLECHANGED") friendlyMessage = "Роль изменена";
    else if (message == "USERDELETED") friendlyMessage = "Пользователь удален";
    else if (message == "SUPPLIERADDED") friendlyMessage = "Поставщик добавлен";
    else if (message == "SUPPLIERUPDATED") friendlyMessage = "Поставщик обновлен";
    else if (message == "SUPPLIERDELETED") friendlyMessage = "Поставщик удален";
    else if (message == "DETAILADDED") friendlyMessage = "Деталь добавлена";
    else if (message == "DETAILUPDATED") friendlyMessage = "Деталь обновлена";
    else if (message == "DETAILDELETED") friendlyMessage = "Деталь удалена";
    else if (message == "PRICE_CHANGE_ADDED") friendlyMessage = "Изменение цены добавлено";
    else if (message == "PRICE_CHANGE_UPDATED") friendlyMessage = "Изменение цены обновлено";
    else if (message == "PRICE_CHANGE_DELETED") friendlyMessage = "Изменение цены удалено";
    else if (message == "SUPPLYADDED") friendlyMessage = "Поставка добавлена";
    else if (message == "SUPPLYUPDATED") friendlyMessage = "Поставка обновлена";
    else if (message == "SUPPLYDELETED") friendlyMessage = "Поставка удалена";
    else if (message == "PASSWORD_CHANGED") friendlyMessage = "Пароль изменен";
    else if (message == "FIO_CHANGED") friendlyMessage = "ФИО изменено";
    else if (message == "ACTIVATED") friendlyMessage = "Активация успешна";
    else friendlyMessage = message; // на случай неизвестного кода

    QMessageBox::information(this, "Успешно", "Операция завершена: " + friendlyMessage);

    // Обновление всех таблиц
    if (m_usersTable) refreshUsersTable();
    if (m_suppliersTable) refreshSuppliersTable();
    if (m_detailsTable) refreshDetailsTable();
    if (m_priceChangesTable) refreshPriceChangesTable();
    if (m_suppliesTable) refreshSuppliesTable();
    if (m_accountingTable) refreshAccountingTable();
    if (m_priceHistoryTable) refreshPriceHistoryTable();
    if (m_currentPricesTable) refreshCurrentPricesTable();
}

void MainWindow::onOperationError(quint32 /*requestId*/, const QString& error)
{
    QMessageBox::critical(this, "Ошибка", error);
}

// ----------------------------------------------------------------
// Периодическое обновление
// ----------------------------------------------------------------

void MainWindow::onPeriodicRefresh()
{
    if (m_currentPricesTable) refreshCurrentPricesTable();
    if (m_priceHistoryTable) refreshPriceHistoryTable();
    if (m_accountingTable) refreshAccountingTable();
}