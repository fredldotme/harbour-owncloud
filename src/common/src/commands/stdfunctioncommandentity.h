#ifndef STDFUNCTIONCOMMANDENTITY_H
#define STDFUNCTIONCOMMANDENTITY_H

#include <QObject>
#include "commandentity.h"
#include <functional>

class StdFunctionCommandEntity : public CommandEntity
{
    Q_OBJECT

public:
    explicit StdFunctionCommandEntity(QObject *parent = Q_NULLPTR,
                                      std::function<void()> function = [](){});
    void startWork();

private:
    std::function<void()> m_function;
};

#endif // STDFUNCTIONCOMMANDENTITY_H
