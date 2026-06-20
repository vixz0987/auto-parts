#ifndef PRICECHANGEDIALOG_H
#define PRICECHANGEDIALOG_H

#include <QDialog>
#include <QList>
#include <QDate>
#include "Utils/datatypes.h"

namespace Ui { class PriceChangeDialog; }

class PriceChangeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PriceChangeDialog(const QList<DetailItem> &details,
                               const QList<SupplierItem> &suppliers,
                               QWidget *parent = nullptr);
    ~PriceChangeDialog();

    int selectedDetailId() const;
    int selectedSupplierId() const;
    QDate changeDate() const;
    double price() const;

    void setDetailId(int id);
    void setSupplierId(int id);
    void setChangeDate(const QDate &date);
    void setPrice(double price);

private slots:
    void onAccept();

private:
    Ui::PriceChangeDialog *ui;
};

#endif