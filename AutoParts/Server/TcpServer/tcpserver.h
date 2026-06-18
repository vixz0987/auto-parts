#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(quint16 port, QObject *parent = nullptr);
    bool start();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
    // буфер для каждого сокета
    QMap<QTcpSocket*, QByteArray> m_buffers;

    // processCommand принимает reqId, команду и аргументы, возвращает полный ответ
    QString processCommand(quint32 reqId, const QString &command, const QString &args);
};

#endif // TCPSERVER_H