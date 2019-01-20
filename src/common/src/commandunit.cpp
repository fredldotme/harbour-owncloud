#include "commandunit.h"

#include <QTimer>
#include <QDebug>

CommandUnit::CommandUnit(QObject *parent,
                         std::initializer_list<CommandEntity*> commands,
                         CommandEntityInfo commandInfo) :
    CommandEntity(parent, commandInfo),
    m_queue(commands),
    m_numProgressingEntities(progressingEntities()),
    m_completedProgressingEntities(0)
{
}

CommandUnit::CommandUnit(QObject *parent,
                         std::deque<CommandEntity*> commands,
                         CommandEntityInfo commandInfo) :
    CommandEntity(parent, commandInfo),
    m_queue(commands),
    m_numProgressingEntities(progressingEntities()),
    m_completedProgressingEntities(0)
{
}

CommandUnit::CommandUnit(QObject* parent,
                         const QQueue<CommandEntity*>& commands,
                         CommandEntityInfo commandInfo) :
    CommandEntity(parent, commandInfo)
{
    for (CommandEntity* command : commands) {
        this->m_queue.push_back(command);
    }

    this->m_numProgressingEntities = progressingEntities();
    this->m_completedProgressingEntities = 0;
}

CommandUnit::~CommandUnit()
{
    abortAllCommands();
}

bool CommandUnit::staticProgress() const
{
    return this->m_numProgressingEntities < 1;
}

void CommandUnit::expand(CommandEntity* previousCommandEntity)
{
    Q_UNUSED(previousCommandEntity);
}

std::deque<CommandEntity*>* CommandUnit::queue()
{
    return &this->m_queue;
}

unsigned int CommandUnit::progressingEntities()
{
    unsigned int count = 0;
    for (const CommandEntity* command : this->m_queue) {
        if (!command)
            continue;

        if (!command->staticProgress())
            count++;
    }
    return count;
}

bool CommandUnit::startWork()
{
    if (!CommandEntity::startWork())
        return false;

    if (this->m_queue.empty()) {
        qWarning() << "CommandUnit is empty";
        return false;
    }

    runFirst();

    setState(RUNNING);
    Q_EMIT started();
    return true;
}

bool CommandUnit::abortWork()
{
    if (this->m_queue.empty())
        return true;

    abortAllCommands();

    Q_EMIT aborted();
    return true;
}

void CommandUnit::runNext()
{
    if (this->m_queue.empty())
        return;

    CommandEntity* command = this->m_queue.front();
    this->m_queue.pop_front();

    if (command) {
        if (!command->staticProgress())
            this->m_completedProgressingEntities++;

        // Decide whether to let additional entities join the
        // queue before running the front entity.
        // This decision has to be based upon the previous CommandEntity.
        // The method is virtual and has to be implemented by inheriting CommandUnit types.
        expand(command);

        qDebug() << "deleting command:" << command << command->state();
        delete command;
        qDebug() << "delete done";
        qDebug() << this->m_queue.size();
    }

    runFirst();
}

void CommandUnit::runFirst()
{
    // Skip nullptrs
    while (!this->m_queue.empty() && !this->m_queue.front()) {
        qDebug() << "popping nullptr from queue";
        this->m_queue.pop_front();
    }

    // All commands successfully run
    if (this->m_queue.empty()) {
        Q_EMIT done();
        return;
    }

    QObject::connect(this->m_queue.front(), &CommandEntity::progressChanged,
                     this, &CommandUnit::updateProgress);
    QObject::connect(this->m_queue.front(), &CommandEntity::aborted,
                     this, &CommandUnit::abortWork);
    QObject::connect(this->m_queue.front(), &CommandEntity::done,
                     this, [=]() {
        QTimer::singleShot(0, this, &CommandUnit::runNext);
    });

    this->m_queue.front()->run();
}

void CommandUnit::updateProgress()
{
    if (!this->m_queue.front())
        return;

    if (this->m_numProgressingEntities < 1)
        return;

    qreal partialProgress =
            ((this->m_queue.front()->progress()) / (qreal)this->m_numProgressingEntities);
    qreal progressCompleted =
            ((qreal)this->m_completedProgressingEntities / (qreal)this->m_numProgressingEntities);

    qDebug() << "partial progress:" << partialProgress;

    setProgress((qreal)(partialProgress + progressCompleted));
}

void CommandUnit::abortAllCommands()
{
    while (!this->m_queue.empty()) {
        CommandEntity* command = this->m_queue.front();
        this->m_queue.pop_front();
        if (command) {
            qDebug() << "deleting command:" << command;
            delete command;
        }
    }
}
