#ifndef LOCALSETTINGSSTORAGE_H
#define LOCALSETTINGSSTORAGE_H

#include <QDebug>
#include <QSettings>

#include "istorage.h"

class LocalSettingsStorage : public IStorage
{
public:
    LocalSettingsStorage();
    ~LocalSettingsStorage();

    const QStringList getUUIDs() const override final;
    const QVariantHash getRow(const QString &uuid) const override final;
    void addModifyRow(const QString &uuid, const QVariantHash &value) override final;
    void removeRow(const QString &uuid) override final;

private:
    QSettings *localDB;
};

#endif // LOCALSETTINGSSTORAGE_H
