#ifndef SERVER_H
#define SERVER_H

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <mutex>

#include "JSONProtocol.h"
#include "localsettingsstorage.h"
#include "memorystorage.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(quint16 port);
    ~Server();

private slots:
    void serverNewConnection();

private:
    void addRow();
    void modifyRow(const QString &uuid, const QVariantHash &value);
    void removeRow(const QString &uuid);
    void syncSocket(QTcpSocket *socket);
    void writeToAllSockets(const QByteArray &data);

private:
    QTcpServer server;
    std::map<qintptr, QTcpSocket *> connectedSockets;

    std::mutex mutex;

    LocalSettingsStorage localSettingsStorage;
    MemoryStorage memoryStorage;
};

#endif // SERVER_H
