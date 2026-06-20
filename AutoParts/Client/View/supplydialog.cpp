#include "supplydialog.h"
#include "ui_supplydialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <algorithm>
#include <QSet>

SupplyDialog::SupplyDialog(ClientService* service, QWidget *parent)
    : QDialog(parent), ui(new Ui::SupplyDialog), m_service(service)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    setWindowTitle("Новая поставка");

    ui->comboDetail->setEnabled(false);
    ui->dateEdit->setEnabled(false);
    ui->spinQuantity->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->labelPrice->setText("Цена за единицу (авто): не определена");

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &SupplyDialog::onAccept);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &QDialog::reject);

    connect(ui->comboSupplier, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SupplyDialog::onSupplierChanged);
    connect(ui->comboDetail, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SupplyDialog::onDetailChanged);
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &SupplyDialog::tryAutoSelectPrice);

    connect(m_service, &ClientService::suppliersLoaded, this, &SupplyDialog::onSuppliersLoaded);
    connect(m_service, &ClientService::detailsLoaded, this, &SupplyDialog::onDetailsLoaded);
    connect(m_service, &ClientService::priceChangesLoaded, this, &SupplyDialog::onPriceChangesLoaded);

    m_service->fetchSuppliers();
    m_service->fetchDetails();
    m_service->fetchPriceChanges();
}

SupplyDialog::~SupplyDialog() { delete ui; }

void SupplyDialog::onSuppliersLoaded(const QList<SupplierData>& suppliers)
{
    m_suppliers = suppliers;
    for (const auto& s : m_suppliers)
        ui->comboSupplier->addItem(s.name, s.id);
    checkDataReady();
}

void SupplyDialog::onDetailsLoaded(const QList<DetailData>& details)
{
    m_details = details;
    checkDataReady();
}

void SupplyDialog::onPriceChangesLoaded(const QList<PriceChangeData>& changes)
{
    m_priceChanges = changes;
    checkDataReady();
}

void SupplyDialog::checkDataReady()
{
    if (!m_suppliers.isEmpty() && !m_details.isEmpty() && !m_priceChanges.isEmpty()) {
        m_dataReady = true;
        ui->comboSupplier->setEnabled(true);
    }
}

void SupplyDialog::onSupplierChanged(int /*index*/)
{
    if (!m_dataReady) return;
    ui->comboDetail->setEnabled(true);
    ui->dateEdit->setEnabled(true);
    ui->spinQuantity->setEnabled(true);
    updateDetailCombo();
}

void SupplyDialog::onDetailChanged(int /*index*/)
{
    tryAutoSelectPrice();
}

void SupplyDialog::updateDetailCombo()
{
    ui->comboDetail->clear();
    int supplierId = selectedSupplierId();
    if (supplierId <= 0) return;

    QSet<int> detailIds;
    for (const auto& pc : m_priceChanges) {
        if (pc.supplierId == supplierId)
            detailIds.insert(pc.detailId);
    }
    for (const auto& d : m_details) {
        if (detailIds.contains(d.id)) {
            ui->comboDetail->addItem(QString("%1 (%2)").arg(d.article, d.name), d.id);
        }
    }
    ui->comboDetail->setCurrentIndex(-1);
    ui->labelPrice->setText("Цена за единицу (авто): не определена");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    m_autoPriceChangeId = 0;
}

void SupplyDialog::tryAutoSelectPrice()
{
    int supplierId = selectedSupplierId();
    int detailId = selectedDetailId();
    QDate date = supplyDate();

    if (supplierId <= 0 || detailId <= 0 || !date.isValid()) {
        ui->labelPrice->setText("Цена за единицу (авто): не определена");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        m_autoPriceChangeId = 0;
        return;
    }

    QList<PriceChangeData> candidates;
    for (const auto& pc : m_priceChanges) {
        if (pc.detailId == detailId && pc.supplierId == supplierId && pc.changeDate <= date)
            candidates.append(pc);
    }
    if (candidates.isEmpty()) {
        ui->labelPrice->setText("Цена за единицу (авто): нет подходящего изменения");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        m_autoPriceChangeId = 0;
        return;
    }

    std::sort(candidates.begin(), candidates.end(), [](const PriceChangeData& a, const PriceChangeData& b) {
        if (a.changeDate != b.changeDate) return a.changeDate > b.changeDate;
        return a.id > b.id;
    });

    const PriceChangeData& latest = candidates.first();
    ui->labelPrice->setText(QString("Цена за единицу (авто): %1 (изменение от %2)")
                                .arg(QString::number(latest.price, 'f', 2), latest.changeDate.toString(Qt::ISODate)));
    m_autoPriceChangeId = latest.id;
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void SupplyDialog::onAccept()
{
    if (selectedSupplierId() <= 0 || selectedDetailId() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите поставщика и деталь!");
        return;
    }
    if (m_autoPriceChangeId == 0) {
        QMessageBox::warning(this, "Ошибка", "Не найдено подходящее изменение цены.");
        return;
    }
    if (quantity() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Количество должно быть больше 0!");
        return;
    }
    accept();
}

QDate SupplyDialog::supplyDate() const { return ui->dateEdit->date(); }
int SupplyDialog::quantity() const { return ui->spinQuantity->value(); }
int SupplyDialog::selectedSupplierId() const { return ui->comboSupplier->currentData().toInt(); }
int SupplyDialog::selectedDetailId() const { return ui->comboDetail->currentData().toInt(); }
int SupplyDialog::selectedPriceChangeId() const { return m_autoPriceChangeId; }
void SupplyDialog::setSupplyDate(const QDate& date) { ui->dateEdit->setDate(date); }
void SupplyDialog::setQuantity(int qty) { ui->spinQuantity->setValue(qty); }