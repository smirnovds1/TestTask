#include "server.h"

Server::Server(quint16 port)
{
    // get data from local file
    QFile file("server.db");
    file.open(QFile::ReadWrite);
    QDataStream dataStream(&file);
    dataStream >> container;

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
    QFile file("server.db");
    file.open(QFile::WriteOnly);
    QDataStream dataStream(&file);
    dataStream << container;
    file.close();
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
                    QByteArray data    = socket->readLine();
                    QJsonDocument json = QJsonDocument::fromJson(data);
                    if (json.isObject())
                    {
                        if (json["command"].isString() && json["index"].isDouble() && json["data"].isObject())
                        {
                            uint64_t index = json["index"].toDouble();
                            if (json["command"] == "add")
                            {
                                qDebug() << "socket" << socket->socketDescriptor() << "added row" << index;
                                addData(index, json["data"].toObject());
                            }
                            if (json["command"] == "modify")
                            {
                                qDebug() << "socket" << socket->socketDescriptor() << "modified row" << index;
                                modifyData(index, json["data"].toObject());
                            }
                            if (json["command"] == "remove")
                            {
                                qDebug() << "socket" << socket->socketDescriptor() << "removed row" << index;
                                removeData(index);
                            }
                            if (json["command"] == "sync")
                            {
                                qDebug() << "socket" << socket->socketDescriptor() << "requested sync";
                                syncSocket(socket);
                            }
                        }
                    }
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

void Server::addData(int index, const QJsonObject &data)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (index == 0)
        container.push_front(Person(data.toVariantMap()));
    else
        container.push_back(Person(data.toVariantMap()));

    writeToAllSockets("add", index, data);
}

void Server::modifyData(int index, const QJsonObject &data)
{
    std::lock_guard<std::mutex> lock(mutex);
    container[index].modify(data.toVariantMap());

    writeToAllSockets("modify", index, data);
}

void Server::removeData(int index)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (index <= container.size() && index >= 0)
        container.remove(index);
    else
        return;

    // write to all sockets json to remove index
    for (auto it = connectedSockets.cbegin(); it != connectedSockets.cend(); ++it)
    {
        QJsonDocument json;
        QJsonObject jsonObject;
        jsonObject["command"] = "remove";
        jsonObject["index"]   = QJsonValue(index);
        jsonObject["data"]    = QJsonObject();
        json.setObject(jsonObject);
        it->second->write(json.toJson(QJsonDocument::Compact) + "\n");
    }
}

void Server::syncSocket(QTcpSocket *socket)
{
    std::lock_guard<std::mutex> lock(mutex);
    int i = 0;
    for (auto it = container.cbegin(); it != container.cend(); ++it)
    {
        QJsonDocument json;
        QJsonObject jsonObject;
        jsonObject["command"] = "add";
        jsonObject["index"]   = QJsonValue(i);
        jsonObject["data"]    = QJsonObject::fromVariantMap(container.at(i).toVariantMap());
        json.setObject(jsonObject);
        socket->write(json.toJson(QJsonDocument::Compact) + "\n");
        i++;
    }
}

void Server::writeToAllSockets(const QString &command, int index, const QJsonObject &data)
{
    for (auto it = connectedSockets.cbegin(); it != connectedSockets.cend(); ++it)
    {
        QJsonDocument json;
        QJsonObject jsonObject;
        jsonObject["command"] = command;
        jsonObject["index"]   = QJsonValue(index);
        jsonObject["data"]    = data;
        json.setObject(jsonObject);
        it->second->write(json.toJson(QJsonDocument::Compact) + "\n");
    }
}
