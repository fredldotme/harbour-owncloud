#ifndef COMMANDENTITYINFO_H
#define COMMANDENTITYINFO_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QVariant>

class CommandEntityInfo
{
    Q_GADGET

public:
    CommandEntityInfo() {}
    CommandEntityInfo(QMap<QString, QVariant> properties)
    {
        this->m_properties = properties;
    }
    CommandEntityInfo(const CommandEntityInfo& o) {
        this->m_properties = o.m_properties;
    }

    Q_INVOKABLE QMap<QString, QVariant> properties() const
    {
        return this->m_properties;
    }

private:
    QMap<QString, QVariant> m_properties;
};
Q_DECLARE_METATYPE(CommandEntityInfo)


#endif // COMMANDENTITYINFO_H
