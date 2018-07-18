#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include <QObject>
#include <QDateTime>
#include <QQueue>

#include "commandentity.h"

class CommandReceipt
{
    Q_GADGET

    Q_PROPERTY(CommandEntityInfo info MEMBER info CONSTANT)
    Q_PROPERTY(QDateTime doneTime MEMBER doneTime CONSTANT)
    Q_PROPERTY(bool finished MEMBER finished CONSTANT)

public:
    CommandReceipt() : doneTime(QDateTime::currentDateTime()), finished(false) {}
    CommandReceipt(CommandEntityInfo info, QDateTime doneTime, bool finished) :
        info(info), doneTime(doneTime), finished(finished) {}
    const CommandEntityInfo info;
    const QDateTime doneTime;
    const bool finished;
};
Q_DECLARE_METATYPE(CommandReceipt)

class CommandQueue : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)

public:
    explicit CommandQueue(QObject *parent = nullptr);
    ~CommandQueue();

    // Command Queue takes ownership of CommandEntity object
    void enqueue(CommandEntity* command);
    void run();
    void stop();
    bool isRunning() { return this->m_running; }
    bool isEmpty() { return this->m_queue.isEmpty(); }
    const QList<CommandEntityInfo> queueInformation() const;

private:
    void runNextCommand();
    void deleteCurrentCommand();
    void setRunning(bool v);

    QQueue<CommandEntity*> m_queue;
    CommandEntity* m_currentCommand = Q_NULLPTR;
    bool m_running = false;

signals:
    void commandFinished(CommandReceipt receipt);
    void runningChanged();
};
Q_DECLARE_METATYPE(CommandQueue*)

#endif // COMMANDQUEUE_H
