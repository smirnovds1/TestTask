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
        return Person::columnIndexToFieldName(section);
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
            emit socketModifyRow(index.row(), index.column(), value);
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
        emit socketRemoveRow(row + i);
    return true;
}

void AddressBookModel::modelClear()
{
    beginResetModel();
    container.clear();
    endResetModel();
}

void AddressBookModel::modelAddRow(int index, const Person &person)
{
    beginInsertRows(QModelIndex(), index, index);
    container.push_back(person);
    endInsertRows();
}

void AddressBookModel::modelModifyRow(int index, const QVariantMap &value)
{
    for (int i = 0; i < Person::columns.size(); ++i)
        if (container[index].getValueByColumnIndex(i) != value.value(Person::columns[i]))
        {
            container[index].setValueByColumn(i, value.value(Person::columns[i]));
            QModelIndex modelIndex = createIndex(index, i);
            emit dataChanged(modelIndex, modelIndex, {Qt::DisplayRole});
        }
}

void AddressBookModel::modelRemoveRow(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    container.removeAt(index);
    endRemoveRows();
}
