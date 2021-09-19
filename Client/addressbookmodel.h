#ifndef ADDRESSBOOKMODEL_H
#define ADDRESSBOOKMODEL_H

#include <QAbstractTableModel>
#include <QDebug>
#include <QHash>

#include "Person.h"

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

    void modelClear();
    void modelAddRow(const QString &uuid, const QVariantHash &value);
    void modelModifyRow(const QString &uuid, const QVariantHash &value);
    void modelRemoveRow(const QString &uuid);

signals:
    void socketModifyRow(const QString &uuid, int column, const QVariant &value);
    void socketAddRow(int row);
    void socketRemoveRow(const QString &uuid);

private:
    QHash<QString, Person *> container;
    QVector<QString> containerIndex;
};

#endif // ADDRESSBOOKMODEL_H
