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

QList<CommandEntityInfo> CommandQueue::queueInformation()
{
    QMutexLocker locker(&this->m_queueMutex);

    QList<CommandEntityInfo> commandInfos;
    for (const CommandEntity* command : this->m_queue) {
        commandInfos.append(command->info());
    }
    return commandInfos;
}

void CommandQueue::enqueue(CommandEntity *command)
{
    qDebug() << Q_FUNC_INFO << command;

    QMutexLocker locker(&this->m_queueMutex);

    QObject::connect(command, &CommandEntity::started,
                     this, []() {
        qDebug() << "Started";
    });
    QObject::connect(command, &CommandEntity::progressChanged,
                     this, [](qreal progress) {
        qDebug() << "Progress: " << progress;
    });
    QObject::connect(command, &CommandEntity::done,
                     this, [=]() {
        deleteCurrentCommand();
        QTimer::singleShot(0, this, &CommandQueue::runNextCommand);
    });
    QObject::connect(command, &CommandEntity::aborted,
                     this, [=]() {
        deleteCommand(qobject_cast<CommandEntity*>(sender()));
        QTimer::singleShot(0, this, &CommandQueue::runNextCommand);
    });

    this->m_queue.enqueue(command);
    Q_EMIT added(command);
    Q_EMIT queueContentChanged();

    if (this->m_queue.size() <= 1)
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

    if (this->m_queue.isEmpty())
        return;

    setRunning(true);
    QTimer::singleShot(0, this, &CommandQueue::runNextCommand);
}

void CommandQueue::runNextCommand()
{
    QMutexLocker locker(&this->m_queueMutex);

    if (!this->m_running)
        return;

    if (this->m_queue.isEmpty()) {
        stop();
        return;
    }

    // Wait until current command is finished
    if (this->m_queue.head()->isRunning())
        return;

    qInfo() << "Now running:" << this->m_queue.head();
    this->m_queue.head()->run();
}

void CommandQueue::stop()
{
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

    CommandReceipt receipt(command->info(),
                           command->resultData(),
                           QDateTime::currentDateTime(),
                           command->isFinished());
    qDebug() << "Command finished?" << receipt.finished;
    Q_EMIT commandFinished(receipt);
    Q_EMIT removed(command);

    this->m_queue.removeOne(command);
    disconnect(command);
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
    deleteCommand(currentCommand);
}
