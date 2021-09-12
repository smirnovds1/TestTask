#ifndef PERSON_H
#define PERSON_H

#include  <QVariantMap>

struct Person
{
    Person() = default;
    Person(const QString &surname, const QString &name, const QString &patronymic, const QString &sex, const QString &phone)
    : surname(surname), name(name), patronymic(patronymic), sex(sex), phone(phone)
    {
    }
    Person(const QVariantMap &data)
    {
        modify(data);
    }

    const QVariantMap toVariantMap() const
    {
        return {{"surname", surname}, {"name", name}, {"patronymic", patronymic}, {"sex", sex}, {"phone", phone}};
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
            sex = data["sex"].toString();
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
    void setValueByName(const QString &field, const QString &value)
    {
        if (field == "surname")
            surname = value;
        if (field == "name")
            name = value;
        if (field == "patronymic")
            patronymic = value;
        if (field == "sex")
            sex = value;
        if (field == "phone")
            phone = value;
    }
    const QString &getValueByColumnIndex(int column) const
    {
        switch (column)
        {
            case 0: return surname;
            case 1: return name;
            case 2: return patronymic;
            case 3: return sex;
            case 4: return phone;
        }
    }

    QString surname    = "";
    QString name       = "";
    QString patronymic = "";
    QString sex        = "";
    QString phone      = "";
};

inline QDataStream &operator<<(QDataStream &stream, const Person &person)
{
    return stream << person.surname << person.name << person.patronymic << person.sex << person.phone;
}

inline QDataStream &operator>>(QDataStream &stream, Person &person)
{
    return stream >> person.surname >> person.name >> person.patronymic >> person.sex >> person.phone;
}

#endif // PERSON_H
