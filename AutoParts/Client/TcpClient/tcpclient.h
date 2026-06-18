#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(const QString &host, quint16 port, QObject *parent = nullptr);

    quint32 sendCommand(const QString &cmd);
    bool isConnected() const;
    bool waitForConnected(int msec = 3000);

signals:
    void responseReceived(quint32 requestId, const QString &response);
    void connectionError(const QString &error);

private slots:
    void onConnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError error);

private:
    QTcpSocket *m_socket;
    QString m_host;
    quint16 m_port;
    quint32 m_nextId = 1;
    QByteArray m_buffer;
};

#endif // TCPCLIENT_H