#include "server.h"

Server::Server(quint16 port)
{
    // get data from local file
    localDB = new QSettings("server.ini", QSettings::IniFormat, this);
    for (QString uuid : localDB->childGroups())
        container.insert(uuid, readFromLocalDB(uuid));
    //    QFile file("server.db");
    //    file.open(QFile::ReadWrite);
    //    QDataStream dataStream(&file);
    //    dataStream >> container;

    // open up server
    if (server.listen(QHostAddress::Any, port))
        qDebug("Server successfully opened @%i", port);
    else
        qFatal("Server can't open port %i", port);
    connect(&server, &QTcpServer::newConnection, this, &Server::serverNewConnection);
}

#include <iostream>
Server::~Server()
{
    qDebug() << __PRETTY_FUNCTION__;
    std::lock_guard<std::mutex> lock(mutex);
    //    QFile file("server.db");
    //    file.open(QFile::WriteOnly);
    //    QDataStream dataStream(&file);
    //    dataStream << container;
    //    file.close();
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
    Person *person = new Person();
    // сервер задает uuid для элемента контейнера
    QString uuid = QUuid::createUuid().toString();
    container.insert(uuid, person);
    localDBModifyRow(uuid, QVariantHash());
    writeToAllSockets(JSONProtocol::createAddRowWithDataMessage(uuid, QVariantHash()));
}

void Server::modifyRow(const QString &uuid, const QVariantHash &value)
{
    std::lock_guard<std::mutex> lock(mutex);
    container.value(uuid)->modify(value);
    localDBModifyRow(uuid, value);
    writeToAllSockets(JSONProtocol::createModifyRowMessage(uuid, value));
}

void Server::removeRow(const QString &uuid)
{
    std::lock_guard<std::mutex> lock(mutex);
    container.remove(uuid);
    localDBRemoveRow(uuid);
    writeToAllSockets(JSONProtocol::createRemoveRowMessage(uuid));
}

void Server::syncSocket(QTcpSocket *socket)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (auto it = container.cbegin(); it != container.cend(); ++it)
        socket->write(JSONProtocol::createAddRowWithDataMessage(it.key(), it.value()->toVariantHash()));
}

void Server::writeToAllSockets(const QByteArray &data)
{
    for (auto it = connectedSockets.cbegin(); it != connectedSockets.cend(); ++it)
        it->second->write(data);
}

Person *Server::readFromLocalDB(const QString &uuid)
{
    localDB->beginGroup(uuid);
    Person *person = new Person;
    for (QString fieldName : Person::columns)
        person->setValueByName(fieldName, localDB->value(fieldName).toString());
    localDB->endGroup();
    return person;
}

void Server::localDBModifyRow(const QString &uuid, const QVariantHash &value)
{
    localDB->beginGroup(uuid);
    for (auto it = value.cbegin(); it != value.end(); ++it)
        localDB->setValue(it.key(), it.value());
    localDB->endGroup();
    localDB->sync();
}

void Server::localDBRemoveRow(const QString &uuid)
{
    localDB->beginGroup(uuid);
    localDB->remove("");
    localDB->endGroup();
    localDB->sync();
}
