#ifndef MEMORYSTORAGE_H
#define MEMORYSTORAGE_H

#include <QString>

#include "Person.h"
#include "istorage.h"

class MemoryStorage : public IStorage
{
public:
    MemoryStorage() = default;
    void initFromPermanentStorage(const IStorage &other);

    const QStringList getUUIDs() const override final;
    const QVariantHash getRow(const QString &uuid) const override final;
    void addModifyRow(const QString &uuid, const QVariantHash &value) override final;
    void removeRow(const QString &uuid) override final;

private:
    QHash<QString, Person *> container;
};

#endif // MEMORYSTORAGE_H
