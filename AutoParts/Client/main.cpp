#include <QApplication>
#include <QMessageBox>
#include "TcpClient/tcpclient.h"
#include "View/mainwindow.h"
#include "Utils/sessionmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TcpClient client("127.0.0.1", 12345);
    if (!client.waitForConnected(5000)) {
        QMessageBox::critical(nullptr, "Ошибка",
                              "Не удалось подключиться к серверу.\n"
                              "Убедитесь, что AutoPartsServer.exe запущен!");
        return 1;
    }

    MainWindow w(&client);
    w.show();

    int ret = a.exec();
    SessionManager::destroy();
    return ret;
}