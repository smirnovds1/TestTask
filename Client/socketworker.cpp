#include "socketworker.h"

SocketWorker::SocketWorker(QObject *parent) : QObject(parent)
{
    connect(&socket, &QTcpSocket::readyRead, this, &SocketWorker::socketReadyRead);
    connect(&socket, &QTcpSocket::stateChanged, this, &SocketWorker::socketStateChanged);
    QTimer::singleShot(0, this, [this]() { this->socketStateChanged(QTcpSocket::SocketState::UnconnectedState); });

    connect(&socketSpeedTimer, &QTimer::timeout, this, &SocketWorker::socketSpeedTimerTimeout);
    QTimer::singleShot(0, this, &SocketWorker::socketSpeedTimerTimeout);
    socketSpeedTimer.start(1000);
}

void SocketWorker::socketModifyRow(const QString &uuid, int column, const QVariant &value)
{
    writeToSocket(JSONProtocol::createModifyRowMessage(uuid, QVariantHash({{Person::columns[column], value}})));
}

void SocketWorker::socketAddRow()
{
    writeToSocket(JSONProtocol::createAddRowMessage());
}

void SocketWorker::socketRemoveRow(const QString &uuid)
{
    writeToSocket(JSONProtocol::createRemoveRowMessage(uuid));
}

void SocketWorker::addressOrPortChanged(const QString &address, const QString &port)
{
    this->address = address;
    this->port    = port;
    socket.disconnectFromHost();
}

void SocketWorker::socketSync()
{
    emit modelClear();
    writeToSocket(JSONProtocol::createSyncMessage());
}

void SocketWorker::writeToSocket(const QByteArray &data)
{
    if (socket.isWritable())
    {
        qint64 bytesWritten = socket.write(data);
        socketBytesSent += bytesWritten;
        if (bytesWritten != data.size())
            emit errorHappened(QString("SocketWriteError: %1 out of %2 bytes written").arg(bytesWritten).arg(data.size()));
    }
}

void SocketWorker::socketSpeedTimerTimeout()
{
    emit socketSpeedChanged(QString("in: %1 B/sec out: %2 B/sec").arg(socketBytesReceived).arg(socketBytesSent));
    socketBytesReceived = 0;
    socketBytesSent     = 0;
}

void SocketWorker::socketStateChanged(QAbstractSocket::SocketState state)
{
    emit socketStatusChanged(QMetaEnum::fromType<QAbstractSocket::SocketState>().valueToKey(state));
    switch (state)
    {
        case QAbstractSocket::UnconnectedState:
            socket.connectToHost(address, port.toUInt());
            break;
        case QAbstractSocket::HostLookupState:
            break;
        case QAbstractSocket::ConnectingState:
            break;
        case QAbstractSocket::ConnectedState:
            socketSync();
            break;
        case QAbstractSocket::BoundState:
            break;
        case QAbstractSocket::ListeningState:
            break;
        case QAbstractSocket::ClosingState:
            break;
    }
}

void SocketWorker::socketReadyRead()
{
    while (socket.canReadLine())
    {
        QByteArray data = socket.readLine();
        socketBytesReceived += data.size();
        JSONProtocol jsonProtocol(data);
        if (jsonProtocol.getCommand() == "add")
            emit modelAddRow(jsonProtocol.getUUID(), jsonProtocol.getData());
        if (jsonProtocol.getCommand() == "modify")
            emit modelModifyRow(jsonProtocol.getUUID(), jsonProtocol.getData());
        if (jsonProtocol.getCommand() == "remove")
            emit modelRemoveRow(jsonProtocol.getUUID());
    }
}
