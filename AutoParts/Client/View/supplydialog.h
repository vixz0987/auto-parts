#ifndef SUPPLYDIALOG_H
#define SUPPLYDIALOG_H

#include <QDialog>
#include "Utils/datatypes.h"

namespace Ui { class SupplyDialog; }

class SupplyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SupplyDialog(const SupplyDialogData &data, QWidget *parent = nullptr);
    ~SupplyDialog();

    QDate supplyDate() const;
    int quantity() const;
    int selectedSupplierId() const;
    int selectedDetailId() const;
    int selectedPriceChangeId() const;   // вычисленный ID изменения цены

    void setSupplyDate(const QDate &date);
    void setQuantity(int qty);

private slots:
    void onAccept();
    void onSupplierChanged(int index);
    void onDetailChanged(int index);

private:
    void updateDetailCombo();             // фильтрует детали по выбранному поставщику
    void tryAutoSelectPrice();            // подбирает цену
    Ui::SupplyDialog *ui;
    SupplyDialogData m_data;
    int m_autoPriceChangeId = 0;
};

#endif // SUPPLYDIALOG_H