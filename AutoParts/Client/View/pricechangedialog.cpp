#include "pricechangedialog.h"
#include "ui_pricechangedialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QDate>

PriceChangeDialog::PriceChangeDialog(const QList<DetailItem> &details,
                                     const QList<SupplierItem> &suppliers,
                                     QWidget *parent)
    : QDialog(parent), ui(new Ui::PriceChangeDialog)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    setWindowTitle("Изменение цены");

    for (const auto &d : details)
        ui->comboDetail->addItem(QString("%1 (%2)").arg(d.article, d.name), d.id);
    for (const auto &s : suppliers)
        ui->comboSupplier->addItem(s.name, s.id);

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &PriceChangeDialog::onAccept);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &QDialog::reject);
}

PriceChangeDialog::~PriceChangeDialog() { delete ui; }

int PriceChangeDialog::selectedDetailId() const { return ui->comboDetail->currentData().toInt(); }
int PriceChangeDialog::selectedSupplierId() const { return ui->comboSupplier->currentData().toInt(); }
QDate PriceChangeDialog::changeDate() const { return ui->dateEdit->date(); }
double PriceChangeDialog::price() const { return ui->doubleSpinPrice->value(); }

void PriceChangeDialog::setDetailId(int id) {
    int idx = ui->comboDetail->findData(id);
    if (idx >= 0) ui->comboDetail->setCurrentIndex(idx);
}
void PriceChangeDialog::setSupplierId(int id) {
    int idx = ui->comboSupplier->findData(id);
    if (idx >= 0) ui->comboSupplier->setCurrentIndex(idx);
}
void PriceChangeDialog::setChangeDate(const QDate &date) { ui->dateEdit->setDate(date); }
void PriceChangeDialog::setPrice(double price) { ui->doubleSpinPrice->setValue(price); }

void PriceChangeDialog::onAccept() {
    if (selectedDetailId() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите деталь!");
        return;
    }
    if (selectedSupplierId() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите поставщика!");
        return;
    }
    if (price() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Цена должна быть положительной!");
        return;
    }
    accept();
}