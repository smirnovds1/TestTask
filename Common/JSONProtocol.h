#ifndef JSONPROTOCOL_H
#define JSONPROTOCOL_H

#include <QJsonDocument>
#include <QJsonObject>

#include "Person.h"

struct JSONProtocol
{
    JSONProtocol(const QByteArray &message);
    static QByteArray createAddRowMessage();
    static QByteArray createAddRowWithDataMessage(const QString &uuid, const QVariantHash &value);
    static QByteArray createModifyRowMessage(const QString &uuid, const QVariantHash &value);
    static QByteArray createRemoveRowMessage(const QString &uuid);
    static QByteArray createSyncMessage();

    const QString getCommand();
    const QString getUUID();
    const QVariantHash getData();

    QJsonObject jsonObject;
};

#endif // JSONPROTOCOL_H
