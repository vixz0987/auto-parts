#include <QCoreApplication>
#include "Utils/database.h"
#include "TcpServer/tcpserver.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Database* db = Database::instance();
    if (!db->connectToDatabase()) {
        qCritical() << "Failed to connect to DB";
        return 1;
    }

    TcpServer server(12345);
    if (!server.start()) {
        qCritical() << "Failed to start server on port 12345";
        return 1;
    }

    qInfo() << "Server started on port 12345";
    int ret = a.exec();
    Database::destroy();
    return ret;
}