#ifndef NOPCOMMANDENTITY_H
#define NOPCOMMANDENTITY_H

#include <QObject>
#include "commands/stdfunctioncommandentity.h"

class NopCommandEntity : public StdFunctionCommandEntity
{
    Q_OBJECT
public:
    explicit NopCommandEntity(QObject *parent = Q_NULLPTR,
                              QString type = QStringLiteral("noop"));

};

#endif // NOPCOMMANDENTITY_H
