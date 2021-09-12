#ifndef SERVER_H
#define SERVER_H

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <mutex>

#include "../Common/Person.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(quint16 port);
    ~Server();

private slots:
    void serverNewConnection();

private:
    void addData(int index, const QJsonObject &data);
    void modifyData(int index, const QJsonObject &data);
    void removeData(int index);
    void syncSocket(QTcpSocket *socket);
    void writeToAllSockets(const QString &command, int index, const QJsonObject &data);

private:
    QTcpServer server;
    std::map<qintptr, QTcpSocket *> connectedSockets;

    std::mutex mutex;
    QVector<Person> container;
};

#endif // SERVER_H
