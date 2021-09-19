#include "memorystorage.h"

void MemoryStorage::initFromPermanentStorage(const IStorage &other)
{
    QStringList temp = other.getUUIDs();
    for (const QString &uuid : other.getUUIDs())
    {
        Person *person = new Person(other.getRow(uuid));
        container.insert(uuid, person);
    }
}

const QStringList MemoryStorage::getUUIDs() const
{
    return container.keys();
}

const QVariantHash MemoryStorage::getRow(const QString &uuid) const
{
    return container.value(uuid)->toVariantHash();
}

void MemoryStorage::addModifyRow(const QString &uuid, const QVariantHash &value)
{
    if (container.contains(uuid))
        container.value(uuid)->modify(value);
    else
    {
        Person *person = new Person(value);
        container.insert(uuid, person);
    }
}

void MemoryStorage::removeRow(const QString &uuid)
{
    delete container.take(uuid);
}
