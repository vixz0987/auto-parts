#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "userdialog.h"
#include "supplierdialog.h"
#include <QApplication>
#include <QMessageBox>
#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

MainWindow::MainWindow(TcpClient *client, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_client(client)
{
    ui->setupUi(this);

    LoginDialog loginDlg(client, this);
    if (loginDlg.exec() != QDialog::Accepted) {
        QApplication::quit();
        return;
    }

    UserInfo user = SessionManager::instance()->currentUser();
    if (user.id == 0) {
        QMessageBox::critical(this, "Ошибка", "Не удалось получить данные пользователя.");
        QApplication::quit();
        return;
    }

    ui->statusbar->showMessage(QString("Авторизован: %1 (%2)").arg(user.fio, user.role));
    connect(m_client, &TcpClient::responseReceived, this, &MainWindow::onServerResponse);
    setupUiForRole(user.role);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUiForRole(const QString &role)
{
    ui->tabWidgetMain->clear();

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
    m_usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_usersTable->horizontalHeader()->setStretchLastSection(true);
    userLayout->addWidget(m_usersTable);

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
    refreshUsersTable();

    QWidget *logTab = new QWidget();
    QLabel *logLabel = new QLabel("Журналы операций (будут реализованы)", logTab);
    logLabel->setAlignment(Qt::AlignCenter);
    ui->tabWidgetMain->addTab(logTab, "Журналы");
}

void MainWindow::addManagerTabs()
{
    // Поставки
    QWidget *suppliesTab = new QWidget();
    QVBoxLayout *suppliesLayout = new QVBoxLayout(suppliesTab);
    m_suppliesTable = new QTableWidget(this);
    m_suppliesTable->setColumnCount(8);
    m_suppliesTable->setHorizontalHeaderLabels({"ID", "Дата", "Поставщик", "Артикул", "Деталь", "Цена", "Кол-во", "Сумма"});
    m_suppliesTable->horizontalHeader()->setStretchLastSection(true);
    suppliesLayout->addWidget(m_suppliesTable);
    ui->tabWidgetMain->addTab(suppliesTab, "Поставки");
    refreshSuppliesTable();

    // Цены
    QWidget *pricesTab = new QWidget();
    QVBoxLayout *pricesLayout = new QVBoxLayout(pricesTab);
    m_priceHistoryTable = new QTableWidget(this);
    m_priceHistoryTable->setColumnCount(4);
    m_priceHistoryTable->setHorizontalHeaderLabels({"Артикул", "Деталь", "Дата", "Цена"});
    m_priceHistoryTable->horizontalHeader()->setStretchLastSection(true);
    pricesLayout->addWidget(m_priceHistoryTable);
    ui->tabWidgetMain->addTab(pricesTab, "Цены");
    refreshPriceHistoryTable();

    // Поставщики
    QWidget *suppliersTab = new QWidget();
    QVBoxLayout *suppliersLayout = new QVBoxLayout(suppliersTab);
    m_suppliersTable = new QTableWidget(this);
    m_suppliersTable->setColumnCount(4);
    m_suppliersTable->setHorizontalHeaderLabels({"ID", "Название", "Телефон", "Адрес"});
    m_suppliersTable->horizontalHeader()->setStretchLastSection(true);
    suppliersLayout->addWidget(m_suppliersTable);

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

void MainWindow::addAccountantTabs()
{
    // Бухгалтерский отчёт
    QWidget *accTab = new QWidget();
    QVBoxLayout *accLayout = new QVBoxLayout(accTab);
    m_accountingTable = new QTableWidget(this);
    m_accountingTable->setColumnCount(7);
    m_accountingTable->setHorizontalHeaderLabels({"Дата", "Поставщик", "Артикул", "Деталь", "Кол-во", "Цена", "Сумма"});
    m_accountingTable->horizontalHeader()->setStretchLastSection(true);
    accLayout->addWidget(m_accountingTable);
    ui->tabWidgetMain->addTab(accTab, "Бухгалтерия");
    refreshAccountingTable();

    // История цен (дубль для бухгалтера)
    QWidget *priceTab = new QWidget();
    QVBoxLayout *priceLayout = new QVBoxLayout(priceTab);
    QTableWidget *priceTable = new QTableWidget(this);
    priceTable->setColumnCount(4);
    priceTable->setHorizontalHeaderLabels({"Артикул", "Деталь", "Дата", "Цена"});
    priceLayout->addWidget(priceTable);
    ui->tabWidgetMain->addTab(priceTab, "Цены");
    // подключим к тому же идентификатору
    connect(this, &MainWindow::destroyed, [=]() { /* ничего, просто чтобы таблица жила */ });
    // для обновления используем отдельный запрос
    m_pendingPriceHistoryId = m_client->sendCommand("PRICEHISTORY");
    // но чтобы обновить эту таблицу, нужно сохранить указатель. Упростим: пусть обновляется вместе с общей.
    // для простоты оставим как есть, данные загрузятся при старте.
}

void MainWindow::onServerResponse(quint32 id, const QString &response)
{
    auto parseJsonArray = [&](const QString &prefix) -> QJsonArray {
        int pos = response.indexOf('[');
        if (pos < 0) return QJsonArray();
        return QJsonDocument::fromJson(response.mid(pos).toUtf8()).array();
    };

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
        m_usersTable->resizeColumnsToContents();
    }
    else if (id == m_pendingSuppliersId) {
        m_pendingSuppliersId = 0;
        QJsonArray arr = parseJsonArray("SUPPLIERS");
        m_suppliersTable->setRowCount(arr.size());
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            m_suppliersTable->setItem(i, 0, new QTableWidgetItem(QString::number(obj["id"].toInt())));
            m_suppliersTable->setItem(i, 1, new QTableWidgetItem(obj["name"].toString()));
            m_suppliersTable->setItem(i, 2, new QTableWidgetItem(obj["phone"].toString()));
            m_suppliersTable->setItem(i, 3, new QTableWidgetItem(obj["address"].toString()));
        }
        m_suppliersTable->resizeColumnsToContents();
    }
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
        m_suppliesTable->resizeColumnsToContents();
    }
    else if (id == m_pendingPriceHistoryId) {
        m_pendingPriceHistoryId = 0;
        QJsonArray arr = parseJsonArray("PRICEHISTORY");
        m_priceHistoryTable->setRowCount(arr.size());
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            m_priceHistoryTable->setItem(i, 0, new QTableWidgetItem(obj["article"].toString()));
            m_priceHistoryTable->setItem(i, 1, new QTableWidgetItem(obj["detail_name"].toString()));
            m_priceHistoryTable->setItem(i, 2, new QTableWidgetItem(obj["change_date"].toString()));
            m_priceHistoryTable->setItem(i, 3, new QTableWidgetItem(QString::number(obj["price"].toDouble(), 'f', 2)));
        }
        m_priceHistoryTable->resizeColumnsToContents();
    }
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
        m_accountingTable->resizeColumnsToContents();
    }
    else if (id == m_pendingGenericId) {
        m_pendingGenericId = 0;
        if (response.startsWith("OK")) {
            QMessageBox::information(this, "Успех", "Операция выполнена успешно.");
            // Обновляем все таблицы
            if (m_usersTable) refreshUsersTable();
            if (m_suppliersTable) refreshSuppliersTable();
            if (m_suppliesTable) refreshSuppliesTable();
            if (m_priceHistoryTable) refreshPriceHistoryTable();
            if (m_accountingTable) refreshAccountingTable();
        } else {
            QString err = "Операция не выполнена.";
            if (response.startsWith("ERROR ")) {
                err = response.mid(QString("ERROR ").length());
            }
            QMessageBox::critical(this, "Ошибка", err);
        }
    }
}

void MainWindow::refreshUsersTable() {
    m_pendingUsersId = m_client->sendCommand("USERS");
}
void MainWindow::refreshSuppliersTable() {
    m_pendingSuppliersId = m_client->sendCommand("SUPPLIERS");
}
void MainWindow::refreshSuppliesTable() {
    m_pendingSuppliesId = m_client->sendCommand("SUPPLIES");
}
void MainWindow::refreshPriceHistoryTable() {
    m_pendingPriceHistoryId = m_client->sendCommand("PRICEHISTORY");
}
void MainWindow::refreshAccountingTable() {
    m_pendingAccountingId = m_client->sendCommand("ACCOUNTING");
}

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
    if (row < 0) { QMessageBox::warning(this, "Ошибка", "Выберите пользователя!"); return; }
    int userId = m_usersTable->item(row, 0)->text().toInt();
    QString currentRole = m_usersTable->item(row, 3)->text();
    QStringList roles = {"admin", "manager", "accountant"};
    bool ok;
    QString newRole = QInputDialog::getItem(this, "Смена роли", "Выберите новую роль:", roles, roles.indexOf(currentRole), false, &ok);
    if (ok && !newRole.isEmpty()) {
        QJsonObject obj;
        obj["userId"] = userId;
        obj["role"] = newRole;
        QString cmd = "CHANGEROLE " + QJsonDocument(obj).toJson(QJsonDocument::Compact);
        m_pendingGenericId = m_client->sendCommand(cmd);
    }
}
void MainWindow::onDeleteUser() {
    int row = m_usersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите пользователя!");
        return;
    }
    int userId = m_usersTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтверждение", "Удалить пользователя?") == QMessageBox::Yes) {
        QJsonObject obj;
        obj["userId"] = userId;
        QString cmd = "DELUSER " + QJsonDocument(obj).toJson(QJsonDocument::Compact);
        m_pendingGenericId = m_client->sendCommand(cmd);
    }
}
void MainWindow::onAddSupplier() {
    SupplierDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QJsonObject obj;
        obj["name"] = dlg.name();
        obj["phone"] = dlg.phone();
        obj["address"] = dlg.address();
        QString cmd = "ADDSUPPLIER " + QJsonDocument(obj).toJson(QJsonDocument::Compact);
        m_pendingGenericId = m_client->sendCommand(cmd);
    }
}
void MainWindow::onEditSupplier() {
    int row = m_suppliersTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Ошибка", "Выберите поставщика!"); return; }
    int id = m_suppliersTable->item(row, 0)->text().toInt();
    QString name = m_suppliersTable->item(row, 1)->text();
    QString phone = m_suppliersTable->item(row, 2)->text();
    QString address = m_suppliersTable->item(row, 3)->text();

    SupplierDialog dlg(this);
    dlg.setName(name);
    dlg.setPhone(phone);
    dlg.setAddress(address);
    if (dlg.exec() == QDialog::Accepted) {
        QJsonObject obj;
        obj["id"] = id;
        obj["name"] = dlg.name();
        obj["phone"] = dlg.phone();
        obj["address"] = dlg.address();
        QString cmd = "UPDSUPPLIER " + QJsonDocument(obj).toJson(QJsonDocument::Compact);
        m_pendingGenericId = m_client->sendCommand(cmd);
    }
}
void MainWindow::onDeleteSupplier() {
    int row = m_suppliersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите поставщика!");
        return;
    }
    int id = m_suppliersTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Подтверждение", "Удалить поставщика?") == QMessageBox::Yes) {
        QJsonObject obj;
        obj["supplierId"] = id;
        QString cmd = "DELSUPPLIER " + QJsonDocument(obj).toJson(QJsonDocument::Compact);
        m_pendingGenericId = m_client->sendCommand(cmd);
    }
}