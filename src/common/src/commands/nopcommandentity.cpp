#include "nopcommandentity.h"

const CommandEntityInfo defaultNopInfo(const QString& type)
{
    QMap<QString, QVariant> properties;
    properties.insert("type", type);
    return CommandEntityInfo(properties);
}

NopCommandEntity::NopCommandEntity(QObject *parent, QString type) :
    StdFunctionCommandEntity(parent, [](){}, defaultNopInfo(type))
{

}
