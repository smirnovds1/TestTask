#ifndef PERSON_H
#define PERSON_H

#include  <QVariantMap>
#include <QDataStream>

struct Person
{
    enum class Sex : uint
    {
        Male,
        Female
    };

    Person() = default;
    Person(const QString &surname, const QString &name, const QString &patronymic, const Sex &sex, const QString &phone)
    : surname(surname), name(name), patronymic(patronymic), sex(sex), phone(phone)
    {
    }
    Person(const QVariantMap &data)
    {
        modify(data);
    }

    const QVariantMap toVariantMap() const
    {
        return {{"surname", surname}, {"name", name}, {"patronymic", patronymic}, {"sex", static_cast<int>(sex)}, {"phone", phone}};
    }

    void modify(const QVariantMap &data)
    {
        if (data.contains("surname"))
            surname = data["surname"].toString();
        if (data.contains("name"))
            name = data["name"].toString();
        if (data.contains("patronymic"))
            patronymic = data["patronymic"].toString();
        if (data.contains("sex"))
            sex = static_cast<Sex>(data["sex"].toUInt());
        if (data.contains("phone"))
            phone = data["phone"].toString();
    }
    static int fieldNameToColumnIndex(const QString &field)
    {
        if (field == "surname")
            return 0;
        if (field == "name")
            return 1;
        if (field == "patronymic")
            return 2;
        if (field == "sex")
            return 3;
        if (field == "phone")
            return 4;
        return -1;
    }
    static QString columnIndexToFieldName(int column)
    {
        switch (column)
        {
            case 0:return "surname";
            case 1:return "name";
            case 2:return "patronymic";
            case 3:return "sex";
            case 4:return "phone";
            default:return QString();
        }
    }
    void setValueByName(const QString &field, const QVariant &value)
    {
        if (field == "surname")
            surname = value.toString();
        if (field == "name")
            name = value.toString();
        if (field == "patronymic")
            patronymic = value.toString();
        if (field == "sex")
            sex = static_cast<Sex>(value.toUInt());
        if (field == "phone")
            phone = value.toString();
    }
    const QVariant getValueByColumnIndex(int column) const
    {
        switch (column)
        {
            case 0: return surname;
            case 1: return name;
            case 2: return patronymic;
            case 3: return static_cast<uint>(sex);
            case 4: return phone;
        }
        return QVariant();
    }

    QString surname    = "";
    QString name       = "";
    QString patronymic = "";
    Sex sex        = Sex::Male;
    QString phone      = "";
};

inline QDataStream &operator<<(QDataStream &stream, const Person &person)
{
    return stream << person.surname << person.name << person.patronymic << static_cast<quint8>(person.sex) << person.phone;
}

inline QDataStream &operator>>(QDataStream &stream, Person &person)
{
    return stream >> person.surname >> person.name >> person.patronymic >> person.sex >> person.phone;
}

#endif // PERSON_H
