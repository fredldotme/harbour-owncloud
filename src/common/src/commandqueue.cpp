#include "commandqueue.h"

#include <QTimer>
#include <QDebug>
#include <QMutexLocker>

CommandQueue::CommandQueue(QObject *parent) : QObject(parent),
    m_queueMutex(QMutex::Recursive)
{

}

CommandQueue::~CommandQueue()
{
    QMutexLocker locker(&this->m_queueMutex);

    while(!this->m_queue.isEmpty()) {
        deleteCurrentCommand();
    }
}

void CommandQueue::setImmediate(bool v)
{
    if (this->m_immediate == v)
        return;

    this->m_immediate = v;
    Q_EMIT immediateChanged();
}

bool CommandQueue::immediate()
{
    return this->m_immediate;
}

QVariantList CommandQueue::queueInformation()
{
    QMutexLocker locker(&this->m_queueMutex);

    QVariantList commandInfos;
    for (const CommandEntity* command : this->m_queue) {
        if (!command)
            continue;

        QVariant info;
        info.setValue(command->info());
        commandInfos.append(info);
    }
    return commandInfos;
}

void CommandQueue::enqueue(CommandEntity *command)
{
    if (!command)
        return;

    qDebug() << Q_FUNC_INFO << command;

    QMutexLocker locker(&this->m_queueMutex);

    for (CommandEntity* queuedCommand : this->m_queue) {
        qDebug() << command << queuedCommand;
        if (command == queuedCommand)
            return;
    }

    QObject::connect(command, &CommandEntity::started, this, []() {
        qDebug() << "Started";
    });
    QObject::connect(command, &CommandEntity::progressChanged, this, [command]() {
        if (command) qDebug() << "Progress: " << command->progress();
    });
    QObject::connect(command, &CommandEntity::done, this, [=]() {
        qDebug() << "Command" << command << "done";
        deleteCommand(command);
        QTimer::singleShot(0, this, &CommandQueue::runNextCommand);
    });
    QObject::connect(command, &CommandEntity::aborted, this, [=]() {
        qWarning() << "Command" << command << "aborted";
        deleteCommand(command);
        QTimer::singleShot(0, this, &CommandQueue::runNextCommand);
    });

    this->m_queue.enqueue(command);
    Q_EMIT added(command);
    Q_EMIT queueContentChanged();

    if (this->m_immediate && !isRunning()) {
        QTimer::singleShot(0, this, &CommandQueue::run);
        return;
    }

    if (this->m_queue.size() == 1)
        QTimer::singleShot(0, this, &CommandQueue::runNextCommand);
}

void CommandQueue::enqueue(QList<CommandEntity*> commands)
{
    qDebug() << Q_FUNC_INFO << commands;

    for (CommandEntity* command : commands) {
        enqueue(command);
    }
}

void CommandQueue::enqueue(QVariant command)
{
    qDebug() << Q_FUNC_INFO;

    if (command.canConvert<CommandEntity*>()) {
        CommandEntity* potentialCommand =
                qobject_cast<CommandEntity*>(command.value<QObject*>());
        if (!potentialCommand) {
            qWarning() << "skip invalid command object";
            return;
        }
        enqueue(potentialCommand);
    } else if (command.canConvert<QVariantList>()) {
        QVariantList potentialCommands = command.toList();
        for (const QVariant& ref : potentialCommands) {
            CommandEntity* potentialCommand =
                    qobject_cast<CommandEntity*>(ref.value<QObject*>());
            if (!potentialCommand) {
                qWarning() << "skip invalid command object";
                continue;
            }
            enqueue(potentialCommand);
        }
    } else {
        qWarning() << "no valid command object provided";
    }
}

void CommandQueue::run()
{
    qDebug() << Q_FUNC_INFO;
    QMutexLocker locker(&this->m_queueMutex);

    if (this->m_running)
        return;

    if (this->m_queue.isEmpty()) {
        qInfo() << "Queue is empty, no need to run";
        return;
    }

    setRunning(true);
    QTimer::singleShot(0, this, &CommandQueue::runNextCommand);
    qDebug() << Q_FUNC_INFO << "end";
}

void CommandQueue::runNextCommand()
{
    qDebug() << Q_FUNC_INFO;
    QMutexLocker locker(&this->m_queueMutex);

    if (!this->m_running)
        return;

    if (this->m_queue.isEmpty()) {
        qInfo() << "Queue is empty, no command to run";
        stop();
        return;
    }

    // Wait until current command is finished
    if (this->m_queue.head()->isRunning())
        return;

    qInfo() << "Now running:" << this->m_queue.head();
    Q_EMIT commandStarted(this->m_queue.head());
    this->m_queue.head()->run();
}

void CommandQueue::stop()
{
    qDebug() << Q_FUNC_INFO;
    QMutexLocker locker(&this->m_queueMutex);

    if (!this->m_running)
        return;

    while (!this->m_queue.isEmpty()) {
        deleteCurrentCommand();
    }

    setRunning(false);
}

void CommandQueue::setRunning(bool v)
{
    qDebug() << Q_FUNC_INFO << v;
    QMutexLocker locker(&this->m_queueMutex);

    if (this->m_running == v)
        return;

    this->m_running = v;
    Q_EMIT runningChanged();
}

QVariantList CommandQueue::queue()
{
    QMutexLocker locker(&this->m_queueMutex);

    QVariantList ret;

    for (CommandEntity* command : m_queue) {
        if (!command)
            continue;

        QVariant v;
        v.setValue(command);
        ret.append(v);
    }

    return ret;
}

void CommandQueue::deleteCommand(CommandEntity* command)
{
    if (!command)
        return;

    QMutexLocker locker(&this->m_queueMutex);

    CommandReceipt receipt(*command);

    disconnect(command, 0, 0, 0);

    this->m_queue.removeOne(command);
    Q_EMIT removed(command);

    qDebug() << "Command finished?" << receipt.finished;
    Q_EMIT commandFinished(receipt);

    command->deleteLater();

    Q_EMIT queueContentChanged();

    if (this->m_queue.isEmpty()) qInfo() << "Queue is now empty";
}

void CommandQueue::deleteCurrentCommand()
{
    QMutexLocker locker(&this->m_queueMutex);

    if (this->m_queue.isEmpty())
        return;

    CommandEntity* currentCommand = this->m_queue.head();
    if (currentCommand)
        deleteCommand(currentCommand);
}
