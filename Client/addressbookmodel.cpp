#include "addressbookmodel.h"

AddressBookModel::AddressBookModel(QObject *parent) : QAbstractTableModel(parent)
{
    //    container.append({"surname", "name", "patronymic", "sex", "phone"});
    //    container.append({"surname2", "name2", "patronymic2", "sex2", "phone2"});
    //    container.append({"surname3", "name3", "patronymic3", "sex3", "phone3"});

    connect(&socket, &QTcpSocket::connected, this, &AddressBookModel::socketConnected);
    connect(&socket, &QTcpSocket::disconnected, this, &AddressBookModel::socketDisconnected);
    connect(&socket, &QTcpSocket::readyRead, this, &AddressBookModel::socketReadyRead);

    socketTimer.setInterval(1000);
    connect(&socketTimer, &QTimer::timeout, this, &AddressBookModel::socketTimerTimeout);
    QTimer::singleShot(0, this, &AddressBookModel::socketDisconnected);

    connect(&socketSpeedTimer, &QTimer::timeout, this, &AddressBookModel::socketSpeedTimerTimeout);
    QTimer::singleShot(0, this, &AddressBookModel::socketSpeedTimerTimeout);
    socketSpeedTimer.start(1000);
}

QVariant AddressBookModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal)
        return Person::columnIndexToFieldName(section);
    if (orientation == Qt::Vertical)
        return section;
    return QVariant();
}

int AddressBookModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return container.size();
}

int AddressBookModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 5;
}

QVariant AddressBookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= container.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        const Person &person = container.at(index.row());
        return person.getValueByColumnIndex(index.column());
    }
    return QVariant();
}

bool AddressBookModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        if (data(index, role) != value)
        {
            QJsonDocument json;
            QJsonObject jsonObject;
            jsonObject["command"] = "modify";
            jsonObject["index"]   = index.row();
            QJsonObject dataObject;
            QString fieldName     = Person::columnIndexToFieldName(index.column());
            dataObject[fieldName] = QJsonValue::fromVariant(value);
            jsonObject["data"]    = dataObject;
            json.setObject(jsonObject);
            writeToSocket(json);
            return true;
        }
    }
    return false;
}

Qt::ItemFlags AddressBookModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

bool AddressBookModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    for (int i = 0; i < count; ++i)
    {
        QJsonDocument json;
        QJsonObject jsonObject;
        jsonObject["command"] = "add";
        jsonObject["index"]   = row + i;
        jsonObject["data"]    = QJsonObject();
        json.setObject(jsonObject);
        writeToSocket(json);
    }
    return true;
}

bool AddressBookModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    for (int i = 0; i < count; ++i)
    {
        QJsonDocument json;
        QJsonObject jsonObject;
        jsonObject["command"] = "remove";
        jsonObject["index"]   = row;
        jsonObject["data"]    = QJsonObject();
        json.setObject(jsonObject);
        writeToSocket(json);
    }
    return true;
}

void AddressBookModel::addressOrPortChanged(const QString &address, const QString &port)
{
    this->address = address;
    this->port    = port;
    socket.disconnectFromHost();
}

void AddressBookModel::socketSync()
{
    { // clear current state
        beginRemoveRows(QModelIndex(), 0, container.size() - 1);
        container.clear();
        endRemoveRows();
    }

    QJsonDocument json;
    QJsonObject jsonObject;
    jsonObject["command"] = "sync";
    jsonObject["index"]   = -1;
    jsonObject["data"]    = QJsonObject();
    json.setObject(jsonObject);
    writeToSocket(json);
}

void AddressBookModel::writeToSocket(const QJsonDocument &json)
{
    if (socket.isWritable())
    {
        QByteArray data = json.toJson(QJsonDocument::Compact) + "\n";
        socketBytesSent += data.size();
        socket.write(data);
    }
}

void AddressBookModel::socketTimerTimeout()
{
    if (socket.state() == QTcpSocket::UnconnectedState)
        socket.connectToHost(address, port.toUInt());
}

void AddressBookModel::socketSpeedTimerTimeout()
{
    emit socketSpeedChanged(QString("in: %1 B/sec out: %2 B/sec").arg(socketBytesReceived).arg(socketBytesSent));
    socketBytesReceived = 0;
    socketBytesSent     = 0;
}

void AddressBookModel::socketConnected()
{
    emit socketStatusChanged("Connected");
    socketTimer.stop();
    socketSync();
}

void AddressBookModel::socketDisconnected()
{
    emit socketStatusChanged("Disconnected");
    socketTimer.start();
}

void AddressBookModel::socketReadyRead()
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
                {
                    const int index = json["index"].toInt();
                    beginInsertRows(QModelIndex(), index, index);
                    if (index == 0)
                        container.push_front(Person(json["data"].toObject().toVariantMap()));
                    else
                        container.push_back(Person(json["data"].toObject().toVariantMap()));
                    endInsertRows();
                }
                if (json["command"] == "modify")
                {
                    const int index                 = json["index"].toInt();
                    Person person                   = container.value(index);
                    const QVariantMap &incomingData = json["data"].toObject().toVariantMap();
                    for (auto it = incomingData.cbegin(); it != incomingData.cend(); ++it)
                    {
                        QModelIndex modelIndex = createIndex(index, Person::fieldNameToColumnIndex(it.key()));
                        person.setValueByName(it.key(), it.value().toString());
                        emit dataChanged(modelIndex, modelIndex, {Qt::DisplayRole});
                    }
                    container.replace(index, person);
                }
                if (json["command"] == "remove")
                {
                    const int index = json["index"].toInt();
                    beginRemoveRows(QModelIndex(), index, index);
                    container.removeAt(index);
                    endRemoveRows();
                }
            }
        }
    }
}
