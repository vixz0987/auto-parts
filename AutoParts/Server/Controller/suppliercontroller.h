#ifndef SUPPLIERCONTROLLER_H
#define SUPPLIERCONTROLLER_H

#include <QList>

class Supplier;

// Контроллер для работы с поставщиками
class SupplierController
{
public:
    // Получение всех поставщиков
    static QList<Supplier*> getAllSuppliers();

    // Добавление нового поставщика
    static bool addSupplier(const QString &name, const QString &phone,
                            const QString &address);

    // Обновление данных поставщика
    static bool updateSupplier(int supplierId, const QString &name,
                               const QString &phone, const QString &address);

    // Удаление поставщика
    static bool deleteSupplier(int supplierId);
};

#endif // SUPPLIERCONTROLLER_H