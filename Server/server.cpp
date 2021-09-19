#include "server.h"

Server::Server(quint16 port)
{
    // open up server
    if (server.listen(QHostAddress::Any, port))
        qDebug("Server successfully opened @%i", port);
    else
        qFatal("Server can't open port %i", port);
    connect(&server, &QTcpServer::newConnection, this, &Server::serverNewConnection);
    memoryStorage.initFromPermanentStorage(localSettingsStorage);
}

Server::~Server()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void Server::serverNewConnection()
{
    QTcpSocket *socket                           = server.nextPendingConnection();
    connectedSockets[socket->socketDescriptor()] = socket;
    qDebug() << "socket" << socket->socketDescriptor() << "connected";
    connect(socket,
            &QTcpSocket::readyRead,
            [this, socket]()
            {
                while (socket->canReadLine())
                {
                    QByteArray data = socket->readLine();
                    JSONProtocol jsonProtocol(data);
                    if (jsonProtocol.getCommand() == "add")
                        addRow();
                    if (jsonProtocol.getCommand() == "modify")
                        modifyRow(jsonProtocol.getUUID(), jsonProtocol.getData());
                    if (jsonProtocol.getCommand() == "remove")
                        removeRow(jsonProtocol.getUUID());
                    if (jsonProtocol.getCommand() == "sync")
                        syncSocket(socket);
                }
            });
    connect(socket,
            &QTcpSocket::disconnected,
            [this, socket]()
            {
                for (auto it = connectedSockets.begin(); it != connectedSockets.end(); ++it)
                    if (it->second == socket)
                    {
                        qDebug() << "socket" << it->first << "disconnected";
                        connectedSockets.erase(it->first);
                        socket->deleteLater();
                        return;
                    }
            });
}

void Server::addRow()
{
    std::lock_guard<std::mutex> lock(mutex);
    // сервер задает uuid для элемента контейнера
    QString uuid = QUuid::createUuid().toString();

    memoryStorage.addModifyRow(uuid, QVariantHash());
    localSettingsStorage.addModifyRow(uuid, QVariantHash());
    writeToAllSockets(JSONProtocol::createAddRowWithDataMessage(uuid, QVariantHash()));
}

void Server::modifyRow(const QString &uuid, const QVariantHash &value)
{
    std::lock_guard<std::mutex> lock(mutex);
    memoryStorage.addModifyRow(uuid, value);
    localSettingsStorage.addModifyRow(uuid, value);
    writeToAllSockets(JSONProtocol::createModifyRowMessage(uuid, value));
}

void Server::removeRow(const QString &uuid)
{
    std::lock_guard<std::mutex> lock(mutex);
    memoryStorage.removeRow(uuid);
    localSettingsStorage.removeRow(uuid);
    writeToAllSockets(JSONProtocol::createRemoveRowMessage(uuid));
}

void Server::syncSocket(QTcpSocket *socket)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (const QString &uuid : memoryStorage.getUUIDs())
        socket->write(JSONProtocol::createAddRowWithDataMessage(uuid, memoryStorage.getRow(uuid)));
}

void Server::writeToAllSockets(const QByteArray &data)
{
    for (auto it = connectedSockets.cbegin(); it != connectedSockets.cend(); ++it)
        it->second->write(data);
}
