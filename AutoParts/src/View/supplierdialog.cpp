#include "supplierdialog.h"
#include "ui_supplierdialog.h"
#include <QMessageBox>
#include <QPushButton>

SupplierDialog::SupplierDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SupplierDialog)
{
    ui->setupUi(this);
    setWindowTitle("Поставщик");

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &SupplierDialog::onAccept);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &QDialog::reject);
}

SupplierDialog::~SupplierDialog()
{
    delete ui;
}

QString SupplierDialog::name() const { return ui->lineName->text().trimmed(); }
QString SupplierDialog::phone() const { return ui->linePhone->text().trimmed(); }
QString SupplierDialog::address() const { return ui->lineAddress->text().trimmed(); }

void SupplierDialog::setName(const QString &name) { ui->lineName->setText(name); }
void SupplierDialog::setPhone(const QString &phone) { ui->linePhone->setText(phone); }
void SupplierDialog::setAddress(const QString &address) { ui->lineAddress->setText(address); }

void SupplierDialog::onAccept()
{
    if (name().isEmpty() || phone().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Название и телефон обязательны!");
        return;
    }
    accept();
}