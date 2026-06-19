#include "tcpclient.h"
#include <QDebug>

TcpClient::TcpClient(const QString &host, quint16 port, QObject *parent)
    : QObject(parent), m_socket(new QTcpSocket(this)), m_host(host), m_port(port)
{
    connect(m_socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &TcpClient::onError);
    m_socket->connectToHost(m_host, m_port);
}

bool TcpClient::isConnected() const
{
    return m_socket->state() == QTcpSocket::ConnectedState;
}

bool TcpClient::waitForConnected(int msec)
{
    return m_socket->waitForConnected(msec);
}

quint32 TcpClient::sendCommand(const QString &cmd)
{
    quint32 id = m_nextId++;
    QByteArray packet = QByteArray::number(id) + " " + cmd.toUtf8() + "\n";
    m_socket->write(packet);
    m_socket->flush();
    return id;
}

void TcpClient::onConnected()
{
    qDebug() << "Подключено к серверу";
}

void TcpClient::onReadyRead()
{
    m_buffer.append(m_socket->readAll());
    while (m_buffer.contains('\n')) {
        int idx = m_buffer.indexOf('\n');
        QByteArray line = m_buffer.left(idx).trimmed();
        m_buffer.remove(0, idx + 1);

        QList<QByteArray> parts = line.split(' ');
        if (parts.size() < 2) continue;
        quint32 id = parts[0].toUInt();
        QString payload = QString::fromUtf8(line.mid(parts[0].size() + 1));
        emit responseReceived(id, payload);
    }
}

void TcpClient::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    emit connectionError(m_socket->errorString());
}