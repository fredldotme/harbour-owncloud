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
    CommandEntityInfo(QMap<QString, QVariant> properties) :
        m_properties(properties) {}
    CommandEntityInfo(const CommandEntityInfo& o) :
        m_properties(o.m_properties) {}

    Q_INVOKABLE QMap<QString, QVariant> properties() const
    {
        return this->m_properties;
    }

    Q_INVOKABLE QVariant property(const QString& key) const
    {
        if (!this->m_properties.contains(key))
            return QVariant();
        return this->m_properties[key];
    }

private:
    QMap<QString, QVariant> m_properties;
};
Q_DECLARE_METATYPE(CommandEntityInfo)


#endif // COMMANDENTITYINFO_H
