#include "commandqueue.h"

#include <QTimer>
#include <QDebug>

CommandQueue::CommandQueue(QObject *parent) : QObject(parent)
{

}

CommandQueue::~CommandQueue()
{
    deleteCurrentCommand();
    while(!this->m_queue.isEmpty()) {
        CommandEntity* command = this->m_queue.dequeue();
        disconnect(command);
        command->deleteLater();
    }
}

const QList<CommandEntityInfo> CommandQueue::queueInformation() const
{
    qDebug() << Q_FUNC_INFO;

    QList<CommandEntityInfo> commandInfos;
    if (this->m_currentCommand)
        commandInfos.append(this->m_currentCommand->info());

    for (const CommandEntity* command : this->m_queue) {
        commandInfos.append(command->info());
    }
    return commandInfos;
}

void CommandQueue::enqueue(CommandEntity *command)
{
    qDebug() << Q_FUNC_INFO;

    this->m_queue.enqueue(command);
    if (!this->m_currentCommand)
        QTimer::singleShot(0, this, &CommandQueue::runNextCommand);
}

void CommandQueue::run()
{
    qDebug() << Q_FUNC_INFO;

    if (this->m_running)
        return;

    if (this->m_queue.isEmpty())
        return;

    setRunning(true);
    QTimer::singleShot(0, this, &CommandQueue::runNextCommand);
}

void CommandQueue::runNextCommand()
{
    qDebug() << Q_FUNC_INFO;

    if (!this->m_running)
        return;

    if (this->m_queue.isEmpty())
        return;

    CommandEntity* command = Q_NULLPTR;

    do {
        command = this->m_queue.dequeue();
    } while (!command);

    // Queue is empty, stop CommandQueue work
    if (!command) {
        stop();
        return;
    }

    // Wait until current command is finished
    if (this->m_currentCommand)
        return;

    this->m_currentCommand = command;

    QObject::connect(this->m_currentCommand, &CommandEntity::started,
                     this, []() {
        qDebug() << "Started";
    });
    QObject::connect(this->m_currentCommand, &CommandEntity::progressChanged,
                     this, [](qreal progress) {
        qDebug() << "Progress: " << progress;
    });
    QObject::connect(this->m_currentCommand, &CommandEntity::done,
                     this, [=]() {
        deleteCurrentCommand();
        QTimer::singleShot(0, this, &CommandQueue::runNextCommand);
    });
    QObject::connect(this->m_currentCommand, &CommandEntity::aborted,
                     this, [=]() {
        deleteCurrentCommand();
        QTimer::singleShot(0, this, &CommandQueue::runNextCommand);
    });

    this->m_currentCommand->run();
}

void CommandQueue::stop()
{
    qDebug() << Q_FUNC_INFO;

    if (!this->m_running)
        return;

    if (this->m_currentCommand) {
        this->m_currentCommand->abort();
        deleteCurrentCommand();
    }

    setRunning(false);
}

void CommandQueue::setRunning(bool v)
{
    if (this->m_running == v)
        return;

    this->m_running = v;
    Q_EMIT runningChanged();
}

void CommandQueue::deleteCurrentCommand()
{
    if (this->m_currentCommand) {
        CommandReceipt receipt(this->m_currentCommand->info(),
                               QDateTime::currentDateTime(),
                               this->m_currentCommand->isFinished());
        qDebug() << "Command finished?" << receipt.finished;
        Q_EMIT commandFinished(receipt);

        disconnect(this->m_currentCommand);
        this->m_currentCommand->deleteLater();
        this->m_currentCommand = Q_NULLPTR;
    }
}
