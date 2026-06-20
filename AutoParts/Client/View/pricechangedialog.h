#ifndef PRICECHANGEDIALOG_H
#define PRICECHANGEDIALOG_H

#include <QDialog>
#include <QDate>
#include "Services/ClientService.h"

namespace Ui { class PriceChangeDialog; }

class PriceChangeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PriceChangeDialog(ClientService* service, QWidget *parent = nullptr);
    ~PriceChangeDialog();

    int selectedDetailId() const;
    int selectedSupplierId() const;
    QDate changeDate() const;
    double price() const;

    void setSupplierId(int id);
    void setChangeDate(const QDate& date);
    void setPrice(double price);

private slots:
    void onAccept();
    void onDetailsLoaded(const QList<DetailData>& details);
    void onSuppliersLoaded(const QList<SupplierData>& suppliers);

private:
    Ui::PriceChangeDialog* ui;
    ClientService* m_service;
    bool m_detailsReady = false;
    bool m_suppliersReady = false;
    void tryEnableOk();
};

#endif // PRICECHANGEDIALOG_H