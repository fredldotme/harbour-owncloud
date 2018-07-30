#include "nopcommandentity.h"

NopCommandEntity::NopCommandEntity(QObject *parent, QString type) :
    StdFunctionCommandEntity(parent, [](){}, type)
{

}
