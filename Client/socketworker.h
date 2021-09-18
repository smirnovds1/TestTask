#ifndef SOCKETWORKER_H
#define SOCKETWORKER_H

#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>
#include <QTcpSocket>
#include <QTimer>

#include "../Common/Person.h"
#include "addressbookmodel.h"

class SocketWorker : public QObject
{
    Q_OBJECT
public:
    explicit SocketWorker(QObject *parent = nullptr);

public slots:
    void addressOrPortChanged(const QString &address, const QString &port);
    void socketSync();
    void socketModifyRow(int row, int column, const QVariant &value);
    void socketAddRow(int row);
    void socketRemoveRow(int row);

signals:
    void socketStatusChanged(const QString &status);
    void socketSpeedChanged(const QString &speed);
    void errorHappened(const QString &error);

    void modelClear();
    void modelAddRow(int index, const Person &person);
    void modelModifyRow(int index, const QVariantMap &person);
    void modelRemoveRow(int index);

private slots:
    void socketSpeedTimerTimeout();
    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketReadyRead();
    void writeToSocket(const QJsonDocument &json);

private:
    QTcpSocket socket;
    QString address;
    QString port;
    QTimer socketSpeedTimer;
    uint64_t socketBytesReceived = 0;
    uint64_t socketBytesSent     = 0;
};

#endif // SOCKETWORKER_H
