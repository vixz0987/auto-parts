#ifndef SUPPLYDIALOG_H
#define SUPPLYDIALOG_H

#include <QDialog>
#include <QList>
#include <QDate>
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
    int selectedPriceChangeId() const;

    void setSupplyDate(const QDate &date);
    void setQuantity(int qty);
    void setSupplierId(int id);
    void setPriceChangeId(int id);

private slots:
    void onAccept();

private:
    Ui::SupplyDialog *ui;
};

#endif // SUPPLYDIALOG_H