#ifndef SUPPLIERDIALOG_H
#define SUPPLIERDIALOG_H

#include <QDialog>

namespace Ui {
class SupplierDialog;
}

// Диалог для добавления / редактирования поставщика
class SupplierDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SupplierDialog(QWidget *parent = nullptr);
    ~SupplierDialog();

    QString name() const;
    QString phone() const;
    QString address() const;

    void setName(const QString &name);
    void setPhone(const QString &phone);
    void setAddress(const QString &address);

private slots:
    void onAccept();

private:
    Ui::SupplierDialog *ui;
};

#endif // SUPPLIERDIALOG_H