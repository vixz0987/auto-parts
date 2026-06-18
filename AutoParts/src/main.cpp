#include "View/mainwindow.h"
#include "Utils/database.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Database* db = Database::instance();
    if (!db->connectToDatabase()) {
        QMessageBox::critical(nullptr, "Ошибка",
                              "Не удалось подключиться к базе данных.\nПриложение будет закрыто.");
        return 1;
    }

    MainWindow w;
    w.show();

    int result = a.exec();

    Database::destroy();
    return result;
}