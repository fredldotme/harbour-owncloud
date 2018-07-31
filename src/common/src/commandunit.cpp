#include "commandunit.h"

#include <QTimer>
#include <QDebug>

CommandUnit::CommandUnit(QObject *parent,
                         std::initializer_list<CommandEntity*> commands,
                         CommandEntityInfo commandInfo) :
    CommandEntity(parent, commandInfo), m_queue(commands)
{
}

CommandUnit::CommandUnit(QObject *parent,
                         std::queue<CommandEntity*> commands,
                         CommandEntityInfo commandInfo) :
    CommandEntity(parent, commandInfo), m_queue(commands)
{
}

CommandUnit::CommandUnit(QObject* parent,
                         QQueue<CommandEntity*> commands,
                         CommandEntityInfo commandInfo) :
    CommandEntity(parent, commandInfo)
{
    for (CommandEntity* command : commands) {
        this->m_queue.push(command);
    }
}

CommandUnit::~CommandUnit()
{
    abortAllCommands();
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
    this->m_queue.pop();

    if (command) {
        qDebug() << "deleting command:" << command << command->state();
        delete command;
        qDebug() << "delete done";
    }

    runFirst();
}

void CommandUnit::runFirst()
{
    // Skip nullptrs
    while (!this->m_queue.empty() && !this->m_queue.front()) {
        qDebug() << "popping nullptr from queue";
        this->m_queue.pop();
    }

    // All commands successfully run
    if (this->m_queue.empty()) {
        Q_EMIT done();
        return;
    }

    QObject::connect(this->m_queue.front(), &CommandEntity::aborted,
                     this, &CommandUnit::abortWork);
    QObject::connect(this->m_queue.front(), &CommandEntity::done,
                     this, [=](){
        QTimer::singleShot(0, this, &CommandUnit::runNext);
    });

    this->m_queue.front()->run();
}

void CommandUnit::abortAllCommands()
{
    while (!this->m_queue.empty()) {
        CommandEntity* command = this->m_queue.front();
        this->m_queue.pop();
        if (command) {
            qDebug() << "deleting command:" << command;
            delete command;
        }
    }
}
