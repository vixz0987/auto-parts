#ifndef SUPPLIER_H
#define SUPPLIER_H

#include <QString>
#include <QObject>

// Класс-модель для таблицы auto_parts.suppliers (Поставщики)
class Supplier : public QObject
{
    Q_OBJECT

public:
    explicit Supplier(QObject *parent = nullptr);
    Supplier(int supplierId, const QString &name, const QString &phone,
             const QString &address);

    int supplierId() const;
    QString name() const;
    QString phone() const;
    QString address() const;

    void setName(const QString &name);
    void setPhone(const QString &phone);
    void setAddress(const QString &address);

    // CRUD
    bool save(); // вставка
    bool update(); // обновление
    bool remove(); // удаление
    static Supplier* loadById(int supplierId);
    static QList<Supplier*> loadAll();

private:
    int m_supplierId;
    QString m_name;
    QString m_phone;
    QString m_address;
};

#endif // SUPPLIER_H