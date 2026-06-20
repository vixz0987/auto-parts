#ifndef SUPPLYDIALOG_H
#define SUPPLYDIALOG_H

#include <QDialog>
#include <QDate>
#include "Services/clientservice.h"

namespace Ui { class SupplyDialog; }

class SupplyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SupplyDialog(ClientService* service, QWidget *parent = nullptr);
    ~SupplyDialog();

    QDate supplyDate() const;
    int quantity() const;
    int selectedSupplierId() const;
    int selectedDetailId() const;
    int selectedPriceChangeId() const;

    void setSupplyDate(const QDate& date);
    void setQuantity(int qty);

private slots:
    void onAccept();
    void onSupplierChanged(int index);
    void onDetailChanged(int index);
    void onSuppliersLoaded(const QList<SupplierData>& suppliers);
    void onDetailsLoaded(const QList<DetailData>& details);
    void onPriceChangesLoaded(const QList<PriceChangeData>& changes);

private:
    void updateDetailCombo();
    void tryAutoSelectPrice();
    void checkDataReady();

    Ui::SupplyDialog* ui;
    ClientService* m_service;

    QList<SupplierData> m_suppliers;
    QList<DetailData> m_details;
    QList<PriceChangeData> m_priceChanges;

    int m_autoPriceChangeId = 0;
    bool m_dataReady = false;
};

#endif // SUPPLYDIALOG_H