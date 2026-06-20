#include <QApplication>
#include <QMessageBox>
#include "TcpClient/tcpclient.h"
#include "View/logindialog.h"
#include "View/mainwindow.h"
#include "Services/sessionmanager.h"
#include "Services/ClientService.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TcpClient client("127.0.0.1", 12345);
    if (!client.waitForConnected(5000)) {
        QMessageBox::critical(nullptr, "Error",
                              "Cannot connect to server.\n"
                              "Ensure AutoPartsServer.exe is running!");
        return 1;
    }

    ClientService service(&client);
    LoginDialog loginDlg(&service);
    if (loginDlg.exec() != QDialog::Accepted) {
        return 0;
    }

    MainWindow w(&client);
    w.show();

    int ret = a.exec();
    SessionManager::destroy();
    return ret;
}