#include "stdfunctioncommandentity.h"

StdFunctionCommandEntity::StdFunctionCommandEntity(QObject *parent, std::function<void()> function) :
    CommandEntity(parent), m_function(function)
{}

void StdFunctionCommandEntity::startWork()
{
    setState(RUNNING);
    m_function();
    setState(FINISHED);
}
