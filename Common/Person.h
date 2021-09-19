#ifndef PERSON_H
#define PERSON_H

#include <QDataStream>
#include <QVariantMap>

struct Person
{
    enum class Sex : int32_t
    {
        Undefined = 0x20,   // " "
        Male      = 0x2642, // "♂"
        Female    = 0x2640  // "♀"
    };
    inline static const QVector<QString> columns = {"surname", "name", "patronymic", "sex", "phone"};

    Person() = default;
    Person(const QString &surname, const QString &name, const QString &patronymic, const Sex &sex, const QString &phone)
    : surname(surname), name(name), patronymic(patronymic), sex(sex), phone(phone)
    {
    }
    Person(const QVariantHash &data)
    {
        modify(data);
    }

    const QVariantHash toVariantHash() const
    {
        return {{"surname", surname}, {"name", name}, {"patronymic", patronymic}, {"sex", static_cast<uint>(sex)}, {"phone", phone}};
    }

    void modify(const QVariantHash &data)
    {
        for (auto it = data.cbegin(); it != data.end(); ++it)
            setValueByName(it.key(), it.value());
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
    void setValueByColumn(int index, const QVariant &value)
    {
        switch (index)
        {
            case 0:
                surname = value.toString();
                break;
            case 1:
                name = value.toString();
                break;
            case 2:
                patronymic = value.toString();
                break;
            case 3:
                sex = static_cast<Sex>(value.toUInt());
                break;
            case 4:
                phone = value.toString();
                break;
        }
    }
    const QVariant getValueByName(const QString &field)
    {
        if (field == "surname")
            return surname;
        if (field == "name")
            return name;
        if (field == "patronymic")
            return patronymic;
        if (field == "sex")
            return QString(QChar(static_cast<int>(sex)));
        if (field == "phone")
            return phone;
        return QVariant();
    }
    const QVariant getValueByColumn(int column) const
    {
        switch (column)
        {
            case 0:
                return surname;
            case 1:
                return name;
            case 2:
                return patronymic;
            case 3:
                return QString(QChar(static_cast<int>(sex)));
            case 4:
                return phone;
        }
        return QVariant();
    }

    QString surname    = "";
    QString name       = "";
    QString patronymic = "";
    Sex sex            = Sex::Undefined;
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
