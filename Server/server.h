#ifndef SERVER_H
#define SERVER_H

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <mutex>

#include "JSONProtocol.h"
#include "Person.h"

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
    Person *readFromLocalDB(const QString &uuid);
    void localDBModifyRow(const QString &uuid, const QVariantHash &value);
    void localDBRemoveRow(const QString &uuid);

private:
    QTcpServer server;
    std::map<qintptr, QTcpSocket *> connectedSockets;

    std::mutex mutex;
    QHash<QString, Person *> container;
    QSettings *localDB;
};

#endif // SERVER_H
