#include "localsettingsstorage.h"

LocalSettingsStorage::LocalSettingsStorage() : IStorage()
{
    // get data from local file
    localDB = new QSettings("server.ini", QSettings::IniFormat);
    if (localDB->status() == QSettings::NoError)
        qDebug() << "Server successfully opened server.ini file";
    else
        qDebug("Server can't open server.ini file");
}

LocalSettingsStorage::~LocalSettingsStorage()
{
    delete localDB;
}

const QStringList LocalSettingsStorage::getUUIDs() const
{
    return localDB->childGroups();
}

const QVariantHash LocalSettingsStorage::getRow(const QString &uuid) const
{
    QVariantHash ret;
    localDB->beginGroup(uuid);
    for (QString fieldName : Person::columns)
        ret[fieldName] = localDB->value(fieldName).toString();
    localDB->endGroup();
    if (localDB->status() != QSettings::NoError)
        qDebug() << __PRETTY_FUNCTION__ << "Error occured while reading from server.ini file";
    return ret;
}

void LocalSettingsStorage::addModifyRow(const QString &uuid, const QVariantHash &value)
{
    localDB->beginGroup(uuid);
    for (auto it = value.cbegin(); it != value.end(); ++it)
        localDB->setValue(it.key(), it.value());
    localDB->endGroup();
    localDB->sync();
    if (localDB->status() != QSettings::NoError)
        qDebug() << __PRETTY_FUNCTION__ << "Error occured while writing to server.ini file";
}

void LocalSettingsStorage::removeRow(const QString &uuid)
{
    localDB->beginGroup(uuid);
    localDB->remove("");
    localDB->endGroup();
    localDB->sync();
    if (localDB->status() != QSettings::NoError)
        qDebug() << __PRETTY_FUNCTION__ << "Error occured while writing to server.ini file";
}
