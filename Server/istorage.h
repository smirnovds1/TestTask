#ifndef ISTORAGE_H
#define ISTORAGE_H

#include <QString>
#include <QVariantHash>

#include "Person.h"

class IStorage
{
public:
    virtual ~IStorage();
    virtual const QStringList getUUIDs() const                                = 0;
    virtual const QVariantHash getRow(const QString &uuid) const              = 0;
    virtual void addModifyRow(const QString &uuid, const QVariantHash &value) = 0;
    virtual void removeRow(const QString &uuid)                               = 0;
};

inline IStorage::~IStorage()
{
}

#endif // ISTORAGE_H
