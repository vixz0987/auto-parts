#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "../Utils/sessionmanager.h"
#include <QApplication>
#include <QMessageBox>
#include <QTabWidget>
#include <QLabel>

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
    // Вкладки для администратора: пользователи, журналы, возможно связи
    QWidget *userTab = new QWidget();
    QLabel *label = new QLabel("Управление пользователями (будет реализовано)", userTab);
    label->setAlignment(Qt::AlignCenter);
    ui->tabWidgetMain->addTab(userTab, "Пользователи");

    QWidget *logTab = new QWidget();
    QLabel *logLabel = new QLabel("Журналы операций (будут реализованы)", logTab);
    logLabel->setAlignment(Qt::AlignCenter);
    ui->tabWidgetMain->addTab(logTab, "Журналы");
}

void MainWindow::addManagerTabs()
{
    // Вкладки для менеджера: поставки, цены, поставщики
    QWidget *suppliesTab = new QWidget();
    QLabel *suppliesLabel = new QLabel("Поставки (будет реализовано)", suppliesTab);
    suppliesLabel->setAlignment(Qt::AlignCenter);
    ui->tabWidgetMain->addTab(suppliesTab, "Поставки");

    QWidget *pricesTab = new QWidget();
    QLabel *pricesLabel = new QLabel("История цен (будет реализовано)", pricesTab);
    pricesLabel->setAlignment(Qt::AlignCenter);
    ui->tabWidgetMain->addTab(pricesTab, "Цены");

    QWidget *suppliersTab = new QWidget();
    QLabel *suppliersLabel = new QLabel("Поставщики (будет реализовано)", suppliersTab);
    suppliersLabel->setAlignment(Qt::AlignCenter);
    ui->tabWidgetMain->addTab(suppliersTab, "Поставщики");
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