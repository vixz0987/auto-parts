#include "pricechangedialog.h"
#include "ui_pricechangedialog.h"
#include <QMessageBox>
#include <QPushButton>

PriceChangeDialog::PriceChangeDialog(ClientService* service, QWidget *parent)
    : QDialog(parent), ui(new Ui::PriceChangeDialog), m_service(service)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    setWindowTitle("Изменение цены");

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &PriceChangeDialog::onAccept);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &QDialog::reject);

    connect(m_service, &ClientService::detailsLoaded, this, &PriceChangeDialog::onDetailsLoaded);
    connect(m_service, &ClientService::suppliersLoaded, this, &PriceChangeDialog::onSuppliersLoaded);

    m_service->fetchDetails();
    m_service->fetchSuppliers();
}

PriceChangeDialog::~PriceChangeDialog() { delete ui; }

void PriceChangeDialog::onDetailsLoaded(const QList<DetailData>& details)
{
    for (const auto& d : details)
        ui->comboDetail->addItem(QString("%1 (%2)").arg(d.article, d.name), d.id);
    m_detailsReady = true;
    tryEnableOk();
}

void PriceChangeDialog::onSuppliersLoaded(const QList<SupplierData>& suppliers)
{
    for (const auto& s : suppliers)
        ui->comboSupplier->addItem(s.name, s.id);
    m_suppliersReady = true;
    tryEnableOk();
}

void PriceChangeDialog::tryEnableOk()
{
    if (m_detailsReady && m_suppliersReady)
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

int PriceChangeDialog::selectedDetailId() const { return ui->comboDetail->currentData().toInt(); }
int PriceChangeDialog::selectedSupplierId() const { return ui->comboSupplier->currentData().toInt(); }
QDate PriceChangeDialog::changeDate() const { return ui->dateEdit->date(); }
double PriceChangeDialog::price() const { return ui->doubleSpinPrice->value(); }

void PriceChangeDialog::setSupplierId(int id) {
    int idx = ui->comboSupplier->findData(id);
    if (idx >= 0) ui->comboSupplier->setCurrentIndex(idx);
}
void PriceChangeDialog::setChangeDate(const QDate& date) { ui->dateEdit->setDate(date); }
void PriceChangeDialog::setPrice(double price) { ui->doubleSpinPrice->setValue(price); }

void PriceChangeDialog::onAccept()
{
    if (selectedDetailId() <= 0 || selectedSupplierId() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите деталь и поставщика!");
        return;
    }
    if (price() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Цена должна быть положительной!");
        return;
    }
    accept();
}