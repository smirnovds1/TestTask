#ifndef SOCKETWORKER_H
#define SOCKETWORKER_H

#include <QHostAddress>
#include <QMetaEnum>
#include <QTcpSocket>
#include <QTimer>

#include "JSONProtocol.h"
#include "Person.h"
#include "addressbookmodel.h"

class SocketWorker : public QObject
{
    Q_OBJECT
public:
    explicit SocketWorker(QObject *parent = nullptr);

public slots:
    void addressOrPortChanged(const QString &address, const QString &port);

    void socketSync();
    void socketModifyRow(const QString &uuid, int column, const QVariant &value);
    void socketAddRow();
    void socketRemoveRow(const QString &uuid);

signals:
    void socketStatusChanged(const QString &status);
    void socketSpeedChanged(const QString &speed);
    void errorHappened(const QString &error);

    void modelClear();
    void modelAddRow(const QString &uuid, const QVariantHash &value);
    void modelModifyRow(const QString &uuid, const QVariantHash &value);
    void modelRemoveRow(const QString &uuid);

private slots:
    void socketSpeedTimerTimeout();
    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketReadyRead();
    void writeToSocket(const QByteArray &data);

private:
    QTcpSocket socket;
    QString address;
    QString port;
    QTimer socketSpeedTimer;
    uint64_t socketBytesReceived = 0;
    uint64_t socketBytesSent     = 0;
};

#endif // SOCKETWORKER_H
