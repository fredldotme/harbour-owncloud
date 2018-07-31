#ifndef COMMANDUNIT_H
#define COMMANDUNIT_H

#include <QObject>
#include <initializer_list>
#include <queue>
#include "commandentity.h"

#include <QQueue>

class CommandUnit : public CommandEntity
{
    Q_OBJECT
public:
    explicit CommandUnit(QObject *parent = Q_NULLPTR,
                         std::initializer_list<CommandEntity*> commands =
            std::initializer_list<CommandEntity*>(),
                         CommandEntityInfo commandInfo =
            CommandEntityInfo());

    explicit CommandUnit(QObject *parent = Q_NULLPTR,
                         std::queue<CommandEntity*> commands =
            std::queue<CommandEntity*>(),
                         CommandEntityInfo commandInfo =
            CommandEntityInfo());

    explicit CommandUnit(QObject *parent = Q_NULLPTR,
                         QQueue<CommandEntity*> commands =
            QQueue<CommandEntity*>(),
                         CommandEntityInfo commandInfo =
            CommandEntityInfo());

    ~CommandUnit();

protected:
    bool startWork() Q_DECL_OVERRIDE;
    bool abortWork() Q_DECL_OVERRIDE;

private:
    void runNext();
    void runFirst();
    void abortAllCommands();

    std::queue<CommandEntity*> m_queue;
};

#endif // COMMANDUNIT_H
