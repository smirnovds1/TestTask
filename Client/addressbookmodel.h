#ifndef ADDRESSBOOKMODEL_H
#define ADDRESSBOOKMODEL_H

#include <QAbstractTableModel>
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <QTimer>

#include "../Common/Person.h"

class AddressBookModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit AddressBookModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

public slots:
    void addressOrPortChanged(const QString &address, const QString &port);
    void socketSync();

signals:
    void socketStatusChanged(const QString &status);
    void socketSpeedChanged(const QString &speed);

private slots:
    void socketTimerTimeout();
    void socketSpeedTimerTimeout();
    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();
    void writeToSocket(const QJsonDocument &json);

private:
    QVector<Person> container;

    QTimer socketTimer;
    QTcpSocket socket;
    QString address;
    QString port;
    QTimer socketSpeedTimer;
    uint64_t socketBytesReceived = 0;
    uint64_t socketBytesSent     = 0;
};

#endif // ADDRESSBOOKMODEL_H
