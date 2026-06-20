#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "userdialog.h"
#include "supplierdialog.h"
#include "detaildialog.h"
#include "pricechangedialog.h"
#include "supplydialog.h"
#include "profiledialog.h"
#include <QApplication>
#include <QMessageBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCheckBox>
#include <QScrollArea>

// Конструктор / деструктор
MainWindow::MainWindow(TcpClient *client, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_client(client)
{
    ui->setupUi(this);

    UserInfo user = SessionManager::instance()->currentUser();
    if (user.id == 0) {
        QMessageBox::critical(this, "Error", "Нет авторизованного пользователя.");
        QApplication::quit();
        return;
    }

    // Создаём кнопки
    m_profileButton = new QPushButton("Профиль", this);
    m_logoutButton = new QPushButton("Выйти", this);

    // Настраиваем размер (опционально)
    m_profileButton->setFixedSize(80, 25);
    m_logoutButton->setFixedSize(80, 25);

    // Подключаем сигналы
    connect(m_profileButton, &QPushButton::clicked, this, &MainWindow::onProfile);
    connect(m_logoutButton, &QPushButton::clicked, this, &MainWindow::onLogout);

    // Размещаем кнопки в статусбаре (справа)
    ui->statusbar->addPermanentWidget(m_profileButton);
    ui->statusbar->addPermanentWidget(m_logoutButton);

    ui->statusbar->showMessage(QString("User: %1 (%2)").arg(user.fio, user.role));
    connect(m_client, &TcpClient::responseReceived, this, &MainWindow::onServerResponse);
    setupUiForRole(user.role);

    ui->statusbar->addPermanentWidget(m_profileButton);
    ui->statusbar->addPermanentWidget(m_logoutButton);

    // Настройка периодического обновления
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::onPeriodicRefresh);
    m_updateTimer->start(5000);  // каждые 5 секунд
}

// Новый слот: обновляет все доступные таблицы
void MainWindow::onPeriodicRefresh()
{
    // Обновляем только те таблицы, которые существуют (не nullptr)
    if (m_usersTable)          refreshUsersTable();
    if (m_suppliersTable)      refreshSuppliersTable();
    if (m_detailsTable)        refreshDetailsTable();
    if (m_priceChangesTable)   refreshPriceChangesTable();
    if (m_suppliesTable)       refreshSuppliesTable();
    if (m_accountingTable)     refreshAccountingTable();
    if (m_priceHistoryTable)   refreshPriceHistoryTable();
    if (m_currentPricesTable)  refreshCurrentPricesTable();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::onProfile()
{
    UserInfo user = SessionManager::instance()->currentUser();
    ProfileDialog dlg(m_client, user.id, this);
    if (dlg.exec() == QDialog::Accepted) {
        // После успешного сохранения данных в профиле можно обновить статусбар
        UserInfo updatedUser = SessionManager::instance()->currentUser();
        ui->statusbar->showMessage(QString("User: %1 (%2)").arg(updatedUser.fio, updatedUser.role));
    }
}

void MainWindow::onLogout()
{
    SessionManager::instance()->logout();
    // Закрываем главное окно
    this->close();
    QApplication::quit();

}

void MainWindow::addCommonTabs()
{
    // Текущие цены
    QWidget *priceTab = new QWidget();
    QVBoxLayout *priceLayout = new QVBoxLayout(priceTab);

    m_currentPricesTable = new QTableWidget(this);
    m_currentPricesTable->setColumnCount(6);
    m_currentPricesTable->setHorizontalHeaderLabels({"Артикул", "Деталь", "Цена", "Дата изменения", "Поставщик", "ID поставщика"});
    m_currentPricesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_currentPricesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_currentPricesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_currentPricesTable->setWordWrap(true);
    m_currentPricesTable->resizeColumnsToContents();
    m_currentPricesTable->resizeRowsToContents();
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

// Распределение вкладок по ролям
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

// Вкладки администратора
void MainWindow::addAdminTabs()
{
    // Пользователи
    QWidget *userTab = new QWidget();
    QVBoxLayout *userLayout = new QVBoxLayout(userTab);

    m_usersTable = new QTableWidget(this);
    m_usersTable->setColumnCount(4);
    m_usersTable->setHorizontalHeaderLabels({"ID", "Логин", "ФИО", "Роль"});
    m_usersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_usersTable->resizeColumnsToContents();
    m_usersTable->resizeRowsToContents();
    m_usersTable->setWordWrap(true);
    m_usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
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

// Вкладки менеджера (и администратора)
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
    m_detailsTable->resizeColumnsToContents();
    m_detailsTable->resizeRowsToContents();
    m_detailsTable->setWordWrap(true);
    detailsLayout->addWidget(m_detailsTable);

    m_searchDetails = new QLineEdit(); m_searchDetails->setPlaceholderText("Поиск деталей...");
    detailsLayout->insertWidget(0, m_searchDetails);
    connect(m_searchDetails, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_detailsTable, text);
    });

    QWidget *detailsBtns = new QWidget();
    QHBoxLayout *dlBtns = new QHBoxLayout(detailsBtns);
    QPushButton *btnAddDetail = new QPushButton("Добавить");
    QPushButton *btnEditDetail = new QPushButton("Изменить");
    QPushButton *btnDelDetail = new QPushButton("Удалить");
    dlBtns->addWidget(btnAddDetail); dlBtns->addWidget(btnEditDetail); dlBtns->addWidget(btnDelDetail);
    detailsLayout->addWidget(detailsBtns);

    connect(btnAddDetail, &QPushButton::clicked, this, &MainWindow::onAddDetail);
    connect(btnEditDetail, &QPushButton::clicked, this, &MainWindow::onEditDetail);
    connect(btnDelDetail, &QPushButton::clicked, this, &MainWindow::onDeleteDetail);

    ui->tabWidgetMain->addTab(detailsTab, "Детали");
    refreshDetailsTable();

    // ИЗМЕНЕНИЯ ЦЕН
    QWidget *pcTab = new QWidget();
    QVBoxLayout *pcLayout = new QVBoxLayout(pcTab);

    m_priceChangesTable = new QTableWidget(this);
    m_priceChangesTable->setColumnCount(6);
    m_priceChangesTable->setHorizontalHeaderLabels({"ID", "Артикул", "Деталь", "Поставщик", "Дата изм.", "Цена"});
    m_priceChangesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_priceChangesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_priceChangesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_priceChangesTable->resizeColumnsToContents();
    m_priceChangesTable->resizeRowsToContents();
    m_priceChangesTable->setWordWrap(true);
    pcLayout->addWidget(m_priceChangesTable);

    m_searchPriceChanges = new QLineEdit(); m_searchPriceChanges->setPlaceholderText("Поиск изменения...");
    pcLayout->insertWidget(0, m_searchPriceChanges);
    connect(m_searchPriceChanges, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_priceChangesTable, text);
    });

    QWidget *pcBtns = new QWidget();
    QHBoxLayout *pcBL = new QHBoxLayout(pcBtns);
    QPushButton *btnAddPC = new QPushButton("Добавить");
    QPushButton *btnEditPC = new QPushButton("Изменить");
    QPushButton *btnDelPC = new QPushButton("Удалить");
    pcBL->addWidget(btnAddPC); pcBL->addWidget(btnEditPC); pcBL->addWidget(btnDelPC);
    pcLayout->addWidget(pcBtns);

    connect(btnAddPC, &QPushButton::clicked, this, &MainWindow::onAddPriceChange);
    connect(btnEditPC, &QPushButton::clicked, this, &MainWindow::onEditPriceChange);
    connect(btnDelPC, &QPushButton::clicked, this, &MainWindow::onDeletePriceChange);

    ui->tabWidgetMain->addTab(pcTab, "Изменения цен (ред.)");
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
    m_suppliesTable->resizeColumnsToContents();
    m_suppliesTable->resizeRowsToContents();
    m_suppliesTable->setWordWrap(true);
    supLayout->addWidget(m_suppliesTable);

    m_searchSupplies = new QLineEdit(); m_searchSupplies->setPlaceholderText("Поиск поставок...");
    supLayout->insertWidget(0, m_searchSupplies);
    connect(m_searchSupplies, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_suppliesTable, text);
    });

    QWidget *supBtns = new QWidget();
    QHBoxLayout *supBL = new QHBoxLayout(supBtns);
    QPushButton *btnAddSup = new QPushButton("Добавить");
    QPushButton *btnEditSup = new QPushButton("Изменить");
    QPushButton *btnDelSup = new QPushButton("Удалить");
    supBL->addWidget(btnAddSup); supBL->addWidget(btnEditSup); supBL->addWidget(btnDelSup);
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
    suppliersLayout->addWidget(m_suppliersTable);

    m_searchSuppliers = new QLineEdit(); m_searchSuppliers->setPlaceholderText("Поиск поставщиков...");
    suppliersLayout->insertWidget(0, m_searchSuppliers);
    connect(m_searchSuppliers, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_suppliersTable, text);
    });

    QWidget *btnPanel = new QWidget();
    QHBoxLayout *btnLayout = new QHBoxLayout(btnPanel);
    QPushButton *btnAdd = new QPushButton("Добавить");
    QPushButton *btnEdit = new QPushButton("Изменить");
    QPushButton *btnDelete = new QPushButton("Удалить");
    btnLayout->addWidget(btnAdd); btnLayout->addWidget(btnEdit); btnLayout->addWidget(btnDelete);
    suppliersLayout->addWidget(btnPanel);

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddSupplier);
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::onEditSupplier);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteSupplier);

    ui->tabWidgetMain->addTab(suppliersTab, "Поставщики");
    refreshSuppliersTable();
}

// Вкладки бухгалтера
void MainWindow::addAccountantTabs()
{
    // Бухгалтерский отчёт
    QWidget *accTab = new QWidget();
    QVBoxLayout *accLayout = new QVBoxLayout(accTab);

    m_accountingSearch = new QLineEdit();            // поле поиска
    m_accountingSearch->setPlaceholderText("Поиск...");
    accLayout->addWidget(m_accountingSearch);

    m_accountingTable = new QTableWidget(this);
    m_accountingTable->setColumnCount(7);
    m_accountingTable->setHorizontalHeaderLabels({"Дата поставки", "Поставщик", "Артикул", "Деталь", "Кол-во", "Цена", "Сумма"});
    m_accountingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_accountingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_accountingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_accountingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_accountingTable->setWordWrap(true);
    m_accountingTable->verticalHeader()->setVisible(false);
    m_accountingTable->setWordWrap(true);
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
    m_priceHistoryTable->resizeColumnsToContents();
    m_priceHistoryTable->resizeRowsToContents();
    phLayout->addWidget(m_priceHistoryTable);

    m_searchPriceHistory = new QLineEdit();
    m_searchPriceHistory->setPlaceholderText("Поиск в истории цен...");
    phLayout->insertWidget(0, m_searchPriceHistory);
    connect(m_searchPriceHistory, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyFilter(m_priceHistoryTable, text);
    });

    ui->tabWidgetMain->addTab(priceHistoryTab, "История цен");
    refreshPriceHistoryTable();

}

// Вспомогательный фильтр для таблиц
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

// CRUD: Детали
void MainWindow::onAddDetail() {
    DetailDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QJsonObject obj;
        obj["article"] = dlg.article();
        obj["name"] = dlg.name();
        m_pendingGenericId = m_client->sendCommand("ADDDETAIL " + QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}
void MainWindow::onEditDetail() {
    int row = m_detailsTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Error", "Выберите деталь!"); return; }
    int id = m_detailsTable->item(row, 0)->text().toInt();
    DetailDialog dlg(this);
    dlg.setArticle(m_detailsTable->item(row, 1)->text());
    dlg.setName(m_detailsTable->item(row, 2)->text());
    if (dlg.exec() == QDialog::Accepted) {
        QJsonObject obj;
        obj["id"] = id;
        obj["article"] = dlg.article();
        obj["name"] = dlg.name();
        m_pendingGenericId = m_client->sendCommand("UPDDETAIL " + QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}
void MainWindow::onDeleteDetail() {
    int row = m_detailsTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Error", "Выберите деталь!"); return; }
    int id = m_detailsTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтвердить", "Удалить деталь?") == QMessageBox::Yes) {
        QJsonObject obj; obj["detailId"] = id;
        m_pendingGenericId = m_client->sendCommand("DELDETAIL " + QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}
void MainWindow::refreshDetailsTable() {
    m_pendingDetailsId = m_client->sendCommand("DETAILS");
}

// CRUD: Изменения цен
void MainWindow::onAddPriceChange() {
    if (m_detailItems.isEmpty() && m_pendingDetailsId == 0)
        refreshDetailsTable();
    if (m_supplierItems.isEmpty() && m_pendingSuppliersId == 0)
        refreshSuppliersTable();
    if (m_detailItems.isEmpty() || m_supplierItems.isEmpty()) {
        QMessageBox::information(this, "Подождите", "Данные ещё загружаются...");
        return;
    }
    PriceChangeDialog dlg(m_detailItems, m_supplierItems, this);
    if (dlg.exec() == QDialog::Accepted) {
        QJsonObject obj;
        obj["detail_id"] = dlg.selectedDetailId();
        obj["change_date"] = dlg.changeDate().toString(Qt::ISODate);
        obj["price"] = dlg.price();
        obj["supplier_id"] = dlg.selectedSupplierId();
        m_pendingGenericId = m_client->sendCommand("ADD_PRICE_CHANGE " +
                                                   QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}

void MainWindow::onEditPriceChange() {
    int row = m_priceChangesTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Error", "Выберите изменение цены!"); return; }
    int id = m_priceChangesTable->item(row, 0)->text().toInt();

    PriceChangeItem current;
    bool found = false;
    for (const auto &pci : m_priceChangeItems) {
        if (pci.id == id) { current = pci; found = true; break; }
    }
    if (!found) {
        QMessageBox::warning(this, "Error", "Информация об изменениях цен недоступна.");
        return;
    }
    if (m_detailItems.isEmpty() || m_supplierItems.isEmpty()) {
        QMessageBox::warning(this, "Error", "Списки деталей/поставщиков еще не загружены.");
        return;
    }
    PriceChangeDialog dlg(m_detailItems, m_supplierItems, this);
    dlg.setDetailId(current.detailId);
    dlg.setSupplierId(current.supplierId);
    dlg.setChangeDate(QDate::fromString(current.date, Qt::ISODate));
    dlg.setPrice(current.price);
    if (dlg.exec() == QDialog::Accepted) {
        QJsonObject obj;
        obj["id"] = id;
        obj["detail_id"] = dlg.selectedDetailId();
        obj["change_date"] = dlg.changeDate().toString(Qt::ISODate);
        obj["price"] = dlg.price();
        obj["supplier_id"] = dlg.selectedSupplierId();
        m_pendingGenericId = m_client->sendCommand("UPD_PRICE_CHANGE " +
                                                   QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}
void MainWindow::onDeletePriceChange() {
    int row = m_priceChangesTable->currentRow();
    if (row < 0) return;
    int id = m_priceChangesTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтвердить", "Удалить изменение цены?") == QMessageBox::Yes) {
        QJsonObject obj; obj["price_change_id"] = id;
        m_pendingGenericId = m_client->sendCommand("DEL_PRICE_CHANGE " + QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}
void MainWindow::refreshPriceChangesTable() {
    m_pendingPriceChangesId = m_client->sendCommand("PRICECHANGES");
}

// CRUD: Поставки
void MainWindow::onAddSupply() {
    if (m_supplierItems.isEmpty() && m_pendingSuppliersId == 0)
        refreshSuppliersTable();
    if (m_detailItems.isEmpty() && m_pendingDetailsId == 0)
        refreshDetailsTable();
    if (m_priceChangeItems.isEmpty() && m_pendingPriceChangesId == 0)
        refreshPriceChangesTable();
    if (m_supplierItems.isEmpty() || m_detailItems.isEmpty() || m_priceChangeItems.isEmpty()) {
        QMessageBox::information(this, "Подождите", "Данные ещё загружаются...");
        return;
    }

    SupplyDialogData data;
    data.suppliers = m_supplierItems;
    data.details = m_detailItems;
    data.priceChanges = m_priceChangeItems;

    SupplyDialog dlg(data, this);
    if (dlg.exec() == QDialog::Accepted) {
        QJsonObject obj;
        obj["supply_date"] = dlg.supplyDate().toString(Qt::ISODate);
        obj["quantity"] = dlg.quantity();
        obj["price_change_id"] = dlg.selectedPriceChangeId();
        m_pendingGenericId = m_client->sendCommand("ADDSUPPLY " +
                                                   QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}

void MainWindow::onEditSupply() {
    int row = m_suppliesTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Error", "Select a supply!"); return; }
    int supplyId = m_suppliesTable->item(row, 0)->text().toInt();
    QMessageBox::information(this, "Не реализовано", "Редактирование поставок реализовано не полностью. Удалите и создайте заново, пожалуйста.");
}
void MainWindow::onDeleteSupply() {
    int row = m_suppliesTable->currentRow();
    if (row < 0) return;
    int id = m_suppliesTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтвердить", "Удалить поставку?") == QMessageBox::Yes) {
        QJsonObject obj; obj["supply_id"] = id;
        m_pendingGenericId = m_client->sendCommand("DELSUPPLY " + QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}
void MainWindow::refreshSuppliesTable() {
    m_pendingSuppliesId = m_client->sendCommand("SUPPLIES");
}

// CRUD: Пользователи
void MainWindow::onAddUser() {
    UserDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QJsonObject obj;
        obj["login"] = dlg.login();
        obj["fio"] = dlg.fio();
        obj["role"] = dlg.role();
        QString cmd = "ADDUSER " + QJsonDocument(obj).toJson(QJsonDocument::Compact);
        m_pendingGenericId = m_client->sendCommand(cmd);
    }
}
void MainWindow::onChangeRole() {
    int row = m_usersTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Error", "Select a user!"); return; }
    int userId = m_usersTable->item(row, 0)->text().toInt();
    QString currentRole = m_usersTable->item(row, 3)->text();
    QStringList roles = {"admin", "manager", "accountant"};
    bool ok;
    QString newRole = QInputDialog::getItem(this, "Изменить роль", "Выберите роль:", roles, roles.indexOf(currentRole), false, &ok);
    if (ok && !newRole.isEmpty()) {
        QJsonObject obj;
        obj["userId"] = userId;
        obj["role"] = newRole;
        m_pendingGenericId = m_client->sendCommand("CHANGEROLE " + QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}
void MainWindow::onDeleteUser() {
    int row = m_usersTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Error", "Выберите пользователя!"); return; }
    int userId = m_usersTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Confirm", "Delete user?") == QMessageBox::Yes) {
        QJsonObject obj; obj["userId"] = userId;
        m_pendingGenericId = m_client->sendCommand("DELUSER " + QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}
void MainWindow::refreshUsersTable() {
    m_pendingUsersId = m_client->sendCommand("USERS");
}

// CRUD: Поставщики
void MainWindow::onAddSupplier() {
    SupplierDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QJsonObject obj;
        obj["name"] = dlg.name();
        obj["phone"] = dlg.phone();
        obj["address"] = dlg.address();
        m_pendingGenericId = m_client->sendCommand("ADDSUPPLIER " + QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}
void MainWindow::onEditSupplier() {
    int row = m_suppliersTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Error", "Выберите поставщика!"); return; }
    int id = m_suppliersTable->item(row, 0)->text().toInt();
    SupplierDialog dlg(this);
    dlg.setName(m_suppliersTable->item(row, 1)->text());
    dlg.setPhone(m_suppliersTable->item(row, 2)->text());
    dlg.setAddress(m_suppliersTable->item(row, 3)->text());
    if (dlg.exec() == QDialog::Accepted) {
        QJsonObject obj;
        obj["id"] = id;
        obj["name"] = dlg.name();
        obj["phone"] = dlg.phone();
        obj["address"] = dlg.address();
        m_pendingGenericId = m_client->sendCommand("UPDSUPPLIER " + QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}
void MainWindow::onDeleteSupplier() {
    int row = m_suppliersTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Error", "Выберите поставщика!"); return; }
    int id = m_suppliersTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтвердить", "Удалить поставщика?") == QMessageBox::Yes) {
        QJsonObject obj; obj["supplierId"] = id;
        m_pendingGenericId = m_client->sendCommand("DELSUPPLIER " + QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}
void MainWindow::refreshSuppliersTable() {
    m_pendingSuppliersId = m_client->sendCommand("SUPPLIERS");
}

// Обновление таблиц представлений
void MainWindow::refreshAccountingTable() {
    m_pendingAccountingId = m_client->sendCommand("ACCOUNTING");
}
void MainWindow::refreshPriceHistoryTable() {
    m_pendingPriceHistoryId = m_client->sendCommand("PRICEHISTORY");
}

void MainWindow::refreshCurrentPricesTable() {
    m_pendingCurrentPricesId = m_client->sendCommand("CURRENTPRICES");
}

// Централизованная обработка ответов сервера
void MainWindow::onServerResponse(quint32 id, const QString &response)
{
    qDebug() << "=== GOT RESPONSE ===" << id << response.left(80);
    auto parseJsonArray = [&](const QString &) -> QJsonArray {
        int pos = response.indexOf('[');
        if (pos < 0) return QJsonArray();
        return QJsonDocument::fromJson(response.mid(pos).toUtf8()).array();
    };

    // Пользователи
    if (id == m_pendingUsersId) {
        m_pendingUsersId = 0;
        QJsonArray arr = parseJsonArray("USERS");
        m_usersTable->setRowCount(arr.size());
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            m_usersTable->setItem(i, 0, new QTableWidgetItem(QString::number(obj["id"].toInt())));
            m_usersTable->setItem(i, 1, new QTableWidgetItem(obj["login"].toString()));
            m_usersTable->setItem(i, 2, new QTableWidgetItem(obj["fio"].toString()));
            m_usersTable->setItem(i, 3, new QTableWidgetItem(obj["role"].toString()));
        }
    }
    // Поставщики
    else if (id == m_pendingSuppliersId) {
        m_pendingSuppliersId = 0;
        QJsonArray arr = parseJsonArray("SUPPLIERS");
        m_suppliersTable->setRowCount(arr.size());
        m_supplierItems.clear();
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            int supId = obj["id"].toInt();
            QString name = obj["name"].toString();
            m_suppliersTable->setItem(i, 0, new QTableWidgetItem(QString::number(supId)));
            m_suppliersTable->setItem(i, 1, new QTableWidgetItem(name));
            m_suppliersTable->setItem(i, 2, new QTableWidgetItem(obj["phone"].toString()));
            m_suppliersTable->setItem(i, 3, new QTableWidgetItem(obj["address"].toString()));
            m_supplierItems.append({supId, name});
        }
        m_suppliersTable->resizeRowsToContents();
    }
    // Обработка списка изменений цен (для таблицы менеджера)
    else if (id == m_pendingPriceChangesId) {
        m_pendingPriceChangesId = 0;
        QJsonArray arr = parseJsonArray("PRICECHANGES");
        m_priceChangesTable->setRowCount(arr.size());
        m_priceChangeItems.clear();
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            int pcId        = obj["id"].toInt();
            int detailId    = obj["detail_id"].toInt();
            int supplierId  = obj["supplier_id"].toInt();
            QString date    = obj["change_date"].toString();
            double price    = obj["price"].toDouble();
            QString article = obj["article"].toString();
            QString detail  = obj["detail_name"].toString();
            QString supplier= obj["supplier_name"].toString();

            m_priceChangesTable->setItem(i, 0, new QTableWidgetItem(QString::number(pcId)));
            m_priceChangesTable->setItem(i, 1, new QTableWidgetItem(article));
            m_priceChangesTable->setItem(i, 2, new QTableWidgetItem(detail));
            m_priceChangesTable->setItem(i, 3, new QTableWidgetItem(supplier));
            m_priceChangesTable->setItem(i, 4, new QTableWidgetItem(date));
            m_priceChangesTable->setItem(i, 5, new QTableWidgetItem(QString::number(price, 'f', 2)));

            // Кэш для поставок
            m_priceChangeItems.append({pcId, detailId, supplierId, date, price});
        }
        m_priceChangesTable->resizeRowsToContents();
    }
    // Детали
    else if (id == m_pendingDetailsId) {
        m_pendingDetailsId = 0;
        QJsonArray arr = parseJsonArray("DETAILS");
        m_detailsTable->setRowCount(arr.size());
        m_detailItems.clear();
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            int detId = obj["id"].toInt();
            QString article = obj["article"].toString();
            QString name = obj["name"].toString();
            m_detailsTable->setItem(i, 0, new QTableWidgetItem(QString::number(detId)));
            m_detailsTable->setItem(i, 1, new QTableWidgetItem(article));
            m_detailsTable->setItem(i, 2, new QTableWidgetItem(name));
            m_detailItems.append({detId, article, name});
        }
    }
    // Поставки
    else if (id == m_pendingSuppliesId) {
        m_pendingSuppliesId = 0;
        QJsonArray arr = parseJsonArray("SUPPLIES");
        m_suppliesTable->setRowCount(arr.size());
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            m_suppliesTable->setItem(i, 0, new QTableWidgetItem(QString::number(obj["supply_id"].toInt())));
            m_suppliesTable->setItem(i, 1, new QTableWidgetItem(obj["supply_date"].toString()));
            m_suppliesTable->setItem(i, 2, new QTableWidgetItem(obj["supplier_name"].toString()));
            m_suppliesTable->setItem(i, 3, new QTableWidgetItem(obj["article"].toString()));
            m_suppliesTable->setItem(i, 4, new QTableWidgetItem(obj["detail_name"].toString()));
            m_suppliesTable->setItem(i, 5, new QTableWidgetItem(QString::number(obj["price"].toDouble(), 'f', 2)));
            m_suppliesTable->setItem(i, 6, new QTableWidgetItem(QString::number(obj["quantity"].toInt())));
            m_suppliesTable->setItem(i, 7, new QTableWidgetItem(QString::number(obj["total_amount"].toDouble(), 'f', 2)));
        }
    }
    // Бухгалтерия
    else if (id == m_pendingAccountingId) {
        m_pendingAccountingId = 0;
        QJsonArray arr = parseJsonArray("ACCOUNTING");
        m_accountingTable->setRowCount(arr.size());
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            m_accountingTable->setItem(i, 0, new QTableWidgetItem(obj["supply_date"].toString()));
            m_accountingTable->setItem(i, 1, new QTableWidgetItem(obj["supplier_name"].toString()));
            m_accountingTable->setItem(i, 2, new QTableWidgetItem(obj["article"].toString()));
            m_accountingTable->setItem(i, 3, new QTableWidgetItem(obj["detail_name"].toString()));
            m_accountingTable->setItem(i, 4, new QTableWidgetItem(QString::number(obj["quantity"].toInt())));
            m_accountingTable->setItem(i, 5, new QTableWidgetItem(QString::number(obj["unit_price"].toDouble(), 'f', 2)));
            m_accountingTable->setItem(i, 6, new QTableWidgetItem(QString::number(obj["total_amount"].toDouble(), 'f', 2)));
        }
    }
    else if (id == m_pendingPriceHistoryId) {
        m_pendingPriceHistoryId = 0;
        QJsonArray arr = parseJsonArray("PRICEHISTORY");
        m_priceHistoryTable->setRowCount(arr.size());
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            m_priceHistoryTable->setItem(i, 0, new QTableWidgetItem(obj["article"].toString()));
            m_priceHistoryTable->setItem(i, 1, new QTableWidgetItem(obj["detail_name"].toString()));
            m_priceHistoryTable->setItem(i, 2, new QTableWidgetItem(obj["supplier_name"].toString()));
            m_priceHistoryTable->setItem(i, 3, new QTableWidgetItem(obj["change_date"].toString()));
            m_priceHistoryTable->setItem(i, 4, new QTableWidgetItem(QString::number(obj["price"].toDouble(), 'f', 2)));
        }
    }
    else if (id == m_pendingCurrentPricesId) {
        m_pendingCurrentPricesId = 0;
        QJsonArray arr = parseJsonArray("CURRENTPRICES");
        m_currentPricesTable->setRowCount(arr.size());
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            m_currentPricesTable->setItem(i, 0, new QTableWidgetItem(obj["article"].toString()));
            m_currentPricesTable->setItem(i, 1, new QTableWidgetItem(obj["detail_name"].toString()));
            m_currentPricesTable->setItem(i, 2, new QTableWidgetItem(QString::number(obj["price"].toDouble(), 'f', 2)));
            m_currentPricesTable->setItem(i, 3, new QTableWidgetItem(obj["change_date"].toString()));
            m_currentPricesTable->setItem(i, 4, new QTableWidgetItem(obj["supplier_name"].toString()));
            m_currentPricesTable->setItem(i, 5, new QTableWidgetItem(QString::number(obj["supplier_id"].toInt())));
        }
    }
    // Универсальный обработчик операций
    else if (id == m_pendingGenericId) {
        m_pendingGenericId = 0;
        if (response.startsWith("OK")) {
            QMessageBox::information(this, "Успешно", "Операция завершена.");
            // Обновляем все затронутые таблицы
            if (m_usersTable)        refreshUsersTable();
            if (m_suppliersTable)    refreshSuppliersTable();
            if (m_detailsTable)      refreshDetailsTable();
            if (m_priceChangesTable) refreshPriceChangesTable();
            if (m_suppliesTable)     refreshSuppliesTable();
            if (m_accountingTable)   refreshAccountingTable();
            if (m_priceHistoryTable)   refreshPriceHistoryTable();
            if (m_currentPricesTable) refreshCurrentPricesTable();
        } else {
            QString err = "Операция завершилась неудачно.";
            if (response.startsWith("ERROR ")) err = response.mid(6);
            QMessageBox::critical(this, "Error", err);
        }
    }
}
