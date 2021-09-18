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

void SocketWorker::socketModifyRow(int row, int column, const QVariant &value)
{
    QJsonDocument json;
    QJsonObject jsonObject;
    jsonObject["command"] = "modify";
    jsonObject["index"]   = row;
    QJsonObject dataObject;
    QString fieldName     = Person::columnIndexToFieldName(column);
    dataObject[fieldName] = QJsonValue::fromVariant(value);
    jsonObject["data"]    = dataObject;
    json.setObject(jsonObject);
    writeToSocket(json);
}

void SocketWorker::socketAddRow(int row)
{
    QJsonDocument json;
    QJsonObject jsonObject;
    jsonObject["command"] = "add";
    jsonObject["index"]   = row;
    jsonObject["data"]    = QJsonObject();
    json.setObject(jsonObject);
    writeToSocket(json);
}

void SocketWorker::socketRemoveRow(int row)
{
    QJsonDocument json;
    QJsonObject jsonObject;
    jsonObject["command"] = "remove";
    jsonObject["index"]   = row;
    jsonObject["data"]    = QJsonObject();
    json.setObject(jsonObject);
    writeToSocket(json);
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

    QJsonDocument json;
    QJsonObject jsonObject;
    jsonObject["command"] = "sync";
    jsonObject["index"]   = -1;
    jsonObject["data"]    = QJsonObject();
    json.setObject(jsonObject);
    writeToSocket(json);
}

void SocketWorker::writeToSocket(const QJsonDocument &json)
{
    if (socket.isWritable())
    {
        QByteArray data = json.toJson(QJsonDocument::Compact) + "\n";
        socketBytesSent += data.size();
        socket.write(data);
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
        QJsonDocument json = QJsonDocument::fromJson(data);
        // if root is not object = reject
        if (json.isObject())
        {
            // general check
            if (json["command"].isString() && json["index"].isDouble() && json["data"].isObject())
            {
                if (json["command"] == "add")
                    emit modelAddRow(json["index"].toInt(), Person(json["data"].toObject().toVariantMap()));
                if (json["command"] == "modify")
                    emit modelModifyRow(json["index"].toInt(), json["data"].toObject().toVariantMap());
                if (json["command"] == "remove")
                    emit modelRemoveRow(json["index"].toInt());
            }
        }
    }
}
