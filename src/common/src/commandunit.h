#ifndef COMMANDUNIT_H
#define COMMANDUNIT_H

#include <QObject>
#include "commandentity.h"

#include <initializer_list>
#include <deque>

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
                         std::deque<CommandEntity*> commands =
            std::deque<CommandEntity*>(),
                         CommandEntityInfo commandInfo =
            CommandEntityInfo());

    explicit CommandUnit(QObject *parent = Q_NULLPTR,
                         const QQueue<CommandEntity*>& commands =
            QQueue<CommandEntity*>(),
                         CommandEntityInfo commandInfo =
            CommandEntityInfo());

    ~CommandUnit();
    bool staticProgress() const Q_DECL_OVERRIDE;

protected:
    bool startWork() Q_DECL_OVERRIDE;
    bool abortWork() Q_DECL_OVERRIDE;
    virtual void expand(CommandEntity* previousCommandEntity);
    std::deque<CommandEntity*>* queue();

private:
    unsigned int progressingEntities();
    void updateProgress();
    void runNext();
    void runFirst();
    void abortAllCommands();

    std::deque<CommandEntity*> m_queue;
    unsigned int m_numProgressingEntities = 0;
    unsigned int m_completedProgressingEntities = 0;
};

#endif // COMMANDUNIT_H
