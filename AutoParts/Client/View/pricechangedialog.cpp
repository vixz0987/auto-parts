#include "pricechangedialog.h"
#include "ui_pricechangedialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QCheckBox>

PriceChangeDialog::PriceChangeDialog(const QList<DetailItem> &details,
                                     const QList<SupplierItem> &suppliers,
                                     QWidget *parent)
    : QDialog(parent), ui(new Ui::PriceChangeDialog)
{
    ui->setupUi(this);
    setWindowTitle("Изменение цены");

    // Заполняем комбобокс деталей
    for (const auto &d : details) {
        ui->comboDetail->addItem(QString("%1 (%2)").arg(d.article, d.name), d.id);
    }

    // Создаём чекбоксы поставщиков в scroll area
    QVBoxLayout *layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    for (const auto &s : suppliers) {
        QCheckBox *cb = new QCheckBox(s.name, ui->scrollAreaWidgetContents);
        cb->setProperty("supplierId", s.id);
        layout->addWidget(cb);
    }
    ui->scrollAreaWidgetContents->setLayout(layout);

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &PriceChangeDialog::onAccept);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &QDialog::reject);
}

PriceChangeDialog::~PriceChangeDialog() { delete ui; }

int PriceChangeDialog::selectedDetailId() const {
    return ui->comboDetail->currentData().toInt();
}

QDate PriceChangeDialog::changeDate() const {
    return ui->dateEdit->date();
}

double PriceChangeDialog::price() const {
    return ui->doubleSpinPrice->value();
}

QList<int> PriceChangeDialog::selectedSupplierIds() const {
    QList<int> ids;
    QLayout *layout = ui->scrollAreaWidgetContents->layout();
    for (int i = 0; i < layout->count(); ++i) {
        QCheckBox *cb = qobject_cast<QCheckBox*>(layout->itemAt(i)->widget());
        if (cb && cb->isChecked()) {
            ids.append(cb->property("supplierId").toInt());
        }
    }
    return ids;
}

void PriceChangeDialog::setDetailId(int id) {
    int idx = ui->comboDetail->findData(id);
    if (idx >= 0) ui->comboDetail->setCurrentIndex(idx);
}

void PriceChangeDialog::setChangeDate(const QDate &date) {
    ui->dateEdit->setDate(date);
}

void PriceChangeDialog::setPrice(double price) {
    ui->doubleSpinPrice->setValue(price);
}

void PriceChangeDialog::setSelectedSupplierIds(const QList<int> &ids) {
    QLayout *layout = ui->scrollAreaWidgetContents->layout();
    for (int i = 0; i < layout->count(); ++i) {
        QCheckBox *cb = qobject_cast<QCheckBox*>(layout->itemAt(i)->widget());
        if (cb) {
            cb->setChecked(ids.contains(cb->property("supplierId").toInt()));
        }
    }
}

void PriceChangeDialog::onAccept() {
    if (selectedDetailId() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите деталь!");
        return;
    }
    if (price() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Цена должна быть положительной!");
        return;
    }
    accept();
}
