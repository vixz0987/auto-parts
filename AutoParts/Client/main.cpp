#include <QApplication>
#include <QMessageBox>
#include "TcpClient/tcpclient.h"
#include "View/logindialog.h"
#include "View/mainwindow.h"
#include "Utils/sessionmanager.h"

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

    LoginDialog loginDlg(&client);
    if (loginDlg.exec() != QDialog::Accepted) {
        return 0;   // выход без запуска главного окна
    }

    MainWindow w(&client);
    w.show();

    int ret = a.exec();
    SessionManager::destroy();
    return ret;
}
