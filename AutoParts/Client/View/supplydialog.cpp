#include "supplydialog.h"
#include "ui_supplydialog.h"
#include <QMessageBox>
#include <QPushButton>

SupplyDialog::SupplyDialog(const SupplyDialogData &data, QWidget *parent)
    : QDialog(parent), ui(new Ui::SupplyDialog)
{
    ui->setupUi(this);
    setWindowTitle("Поставка");

    for (const auto &s : data.suppliers) {
        ui->comboSupplier->addItem(s.second, s.first);
    }
    for (const auto &pc : data.priceChanges) {
        ui->comboPriceChange->addItem(pc.second, pc.first);
    }

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &SupplyDialog::onAccept);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &QDialog::reject);
}

SupplyDialog::~SupplyDialog() { delete ui; }

QDate SupplyDialog::supplyDate() const { return ui->dateEdit->date(); }
int SupplyDialog::quantity() const { return ui->spinQuantity->value(); }
int SupplyDialog::selectedSupplierId() const { return ui->comboSupplier->currentData().toInt(); }
int SupplyDialog::selectedPriceChangeId() const { return ui->comboPriceChange->currentData().toInt(); }

void SupplyDialog::setSupplyDate(const QDate &date) { ui->dateEdit->setDate(date); }
void SupplyDialog::setQuantity(int qty) { ui->spinQuantity->setValue(qty); }
void SupplyDialog::setSupplierId(int id) {
    int idx = ui->comboSupplier->findData(id);
    if (idx >= 0) ui->comboSupplier->setCurrentIndex(idx);
}
void SupplyDialog::setPriceChangeId(int id) {
    int idx = ui->comboPriceChange->findData(id);
    if (idx >= 0) ui->comboPriceChange->setCurrentIndex(idx);
}

void SupplyDialog::onAccept() {
    if (selectedSupplierId() <= 0 || selectedPriceChangeId() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите поставщика и изменение цены!");
        return;
    }
    if (quantity() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Количество должно быть > 0!");
        return;
    }
    accept();
}
