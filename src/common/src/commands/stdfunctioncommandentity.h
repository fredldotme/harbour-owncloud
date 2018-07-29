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
                                      std::function<void()> function = [](){},
                                      const QString& type = QStringLiteral("")) :
        CommandEntity(parent), m_function(function) {
        if (!type.isEmpty()) {
            QMap<QString, QVariant> commandProperties;
            commandProperties.insert(QStringLiteral("type"), type);
            this->m_commandInfo = CommandEntityInfo(commandProperties);
        }
    }

    bool startWork()
    {
        if (!CommandEntity::startWork())
            return false;

        setState(RUNNING);
        m_function();
        setState(FINISHED);

        Q_EMIT done();
        return true;
    }

    bool abortWork()
    {
        if (!CommandEntity::abortWork())
            return false;

        setState(ABORTED);
        Q_EMIT aborted();
        return true;
    }

private:
    std::function<void()> m_function;
};

#endif // STDFUNCTIONCOMMANDENTITY_H
