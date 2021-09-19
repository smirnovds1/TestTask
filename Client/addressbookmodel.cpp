#include "addressbookmodel.h"

AddressBookModel::AddressBookModel(QObject *parent) : QAbstractTableModel(parent)
{
    //    container.append({"surname", "name", "patronymic", "sex", "phone"});
    //    container.append({"surname2", "name2", "patronymic2", "sex2", "phone2"});
    //    container.append({"surname3", "name3", "patronymic3", "sex3", "phone3"});
}

QVariant AddressBookModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal)
        return Person::columns[section];
    if (orientation == Qt::Vertical)
        return section;
    return QVariant();
}

int AddressBookModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (parent.isValid())
        return 0;
    return container.size();
}

int AddressBookModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (parent.isValid())
        return 0;
    return Person::columns.size();
}

QVariant AddressBookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= container.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        Person *person = container.value(containerIndex.value(index.row()));
        return person->getValueByColumn(index.column());
    }
    return QVariant();
}

bool AddressBookModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        if (data(index, role) != value)
        {
            emit socketModifyRow(containerIndex.value(index.row()), index.column(), value);
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
        emit socketAddRow(row + i);
    return true;
}

bool AddressBookModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    for (int i = 0; i < count; ++i)
        emit socketRemoveRow(containerIndex.value(row + i));
    return true;
}

void AddressBookModel::modelClear()
{
    beginResetModel();
    container.clear();
    containerIndex.clear();
    endResetModel();
}

void AddressBookModel::modelAddRow(const QString &uuid, const QVariantHash &value)
{
    containerIndex.append(uuid);
    const int index = containerIndex.indexOf(uuid);
    Person *person  = new Person(value);
    beginInsertRows(QModelIndex(), index, index);
    container.insert(uuid, person);
    endInsertRows();
}

void AddressBookModel::modelModifyRow(const QString &uuid, const QVariantHash &value)
{
    const int row = containerIndex.indexOf(uuid);
    // проверка на валидность
    if (row < 0)
        return;
    Person *person = container.value(uuid);
    for (auto it = value.cbegin(); it != value.cend(); ++it)
    {
        const int column = Person::columns.indexOf(it.key());
        if (person->getValueByColumn(column) != it.value())
        {
            person->setValueByColumn(column, it.value());
            QModelIndex modelIndex = createIndex(row, column);
            emit dataChanged(modelIndex, modelIndex, {Qt::DisplayRole});
        }
    }
}

void AddressBookModel::modelRemoveRow(const QString &uuid)
{
    const int row = containerIndex.indexOf(uuid);
    // проверка на валидность
    if (row < 0)
        return;
    beginRemoveRows(QModelIndex(), row, row);
    delete container.take(uuid);
    containerIndex.removeAll(uuid);
    endRemoveRows();
}
