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
    QDate changeDate() const;
    double price() const;
    QList<int> selectedSupplierIds() const;

    // для редактирования
    void setDetailId(int id);
    void setChangeDate(const QDate &date);
    void setPrice(double price);
    void setSelectedSupplierIds(const QList<int> &ids);

private slots:
    void onAccept();

private:
    Ui::PriceChangeDialog *ui;
};

#endif // PRICECHANGEDIALOG_H
