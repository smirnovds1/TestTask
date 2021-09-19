#include "JSONProtocol.h"

JSONProtocol::JSONProtocol(const QByteArray &message)
{
    QJsonDocument json = QJsonDocument::fromJson(message);
    if (json.isObject())
        jsonObject = json.object();
}

QByteArray JSONProtocol::createAddRowMessage()
{
    QJsonDocument json;
    QJsonObject jsonObject;
    jsonObject["command"] = "add";
    json.setObject(jsonObject);
    return json.toJson(QJsonDocument::Compact) + "\n";
}

QByteArray JSONProtocol::createAddRowWithDataMessage(const QString &uuid, const QVariantHash &value)
{
    QJsonDocument json;
    QJsonObject jsonObject;
    jsonObject["command"] = "add";
    jsonObject["uuid"]    = uuid;
    jsonObject["data"]    = QJsonObject::fromVariantHash(value);
    json.setObject(jsonObject);
    return json.toJson(QJsonDocument::Compact) + "\n";
}

QByteArray JSONProtocol::createModifyRowMessage(const QString &uuid, const QVariantHash &value)
{
    QJsonDocument json;
    QJsonObject jsonObject;
    jsonObject["command"] = "modify";
    jsonObject["uuid"]    = uuid;
    jsonObject["data"]    = QJsonObject::fromVariantHash(value);
    json.setObject(jsonObject);
    return json.toJson(QJsonDocument::Compact) + "\n";
}

QByteArray JSONProtocol::createRemoveRowMessage(const QString &uuid)
{
    QJsonDocument json;
    QJsonObject jsonObject;
    jsonObject["command"] = "remove";
    jsonObject["uuid"]    = uuid;
    json.setObject(jsonObject);
    return json.toJson(QJsonDocument::Compact) + "\n";
}

QByteArray JSONProtocol::createSyncMessage()
{
    QJsonDocument json;
    QJsonObject jsonObject;
    jsonObject["command"] = "sync";
    json.setObject(jsonObject);
    return json.toJson(QJsonDocument::Compact) + "\n";
}

const QString JSONProtocol::getCommand()
{
    if (jsonObject["command"].isString())
        return jsonObject["command"].toString();
    return QString();
}

const QString JSONProtocol::getUUID()
{
    if (jsonObject["uuid"].isString())
        return jsonObject["uuid"].toString();
    return QString();
}

const QVariantHash JSONProtocol::getData()
{
    if (jsonObject["data"].isObject())
        return jsonObject["data"].toObject().toVariantHash();
    return QVariantHash();
}
