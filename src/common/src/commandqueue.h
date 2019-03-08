#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include <QObject>
#include <QDateTime>
#include <QQueue>
#include <QMutex>

#include "commandentity.h"

class CommandReceipt
{
    Q_GADGET

    Q_PROPERTY(CommandEntityInfo info MEMBER info CONSTANT)
    Q_PROPERTY(QVariant result MEMBER result CONSTANT)
    Q_PROPERTY(QDateTime doneTime MEMBER doneTime CONSTANT)
    Q_PROPERTY(bool finished MEMBER finished CONSTANT)
    Q_PROPERTY(bool abortIntended MEMBER abortIntended CONSTANT)
    Q_PROPERTY(bool valid MEMBER valid CONSTANT)

public:
    CommandReceipt() :
        doneTime(QDateTime::currentDateTime()),
        finished(false),
        abortIntended(false),
        valid(false)
    {}
    CommandReceipt(const CommandEntity& command) :
        info(command.info()), result(command.resultData()),
        doneTime(QDateTime::currentDateTime()),
        finished(command.isFinished()),
        abortIntended(command.abortIntended()),
        valid(true)
    {}

    const CommandEntityInfo info;
    const QVariant result;
    const QDateTime doneTime;
    const bool finished;
    const bool abortIntended;
    const bool valid;
};
Q_DECLARE_METATYPE(CommandReceipt)

class CommandQueue : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool immediate READ immediate WRITE setImmediate NOTIFY immediateChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(QVariantList queue READ queue NOTIFY queueContentChanged)

public:
    explicit CommandQueue(QObject *parent = nullptr);
    ~CommandQueue();

    void setImmediate(bool v);
    bool immediate();

    // Command Queue takes ownership of CommandEntity object
    void enqueue(CommandEntity* command);
    void enqueue(QList<CommandEntity*> commands);

public slots:
    void enqueue(QVariant command);
    void run();
    void stop();
    bool isRunning() { return this->m_running; }
    bool isEmpty() { return this->m_queue.isEmpty(); }
    QVariantList queue();
    QVariantList queueInformation();

private:
    void runNextCommand();
    void deleteCurrentCommand();
    void deleteCommand(CommandEntity* command = Q_NULLPTR);
    void setRunning(bool v);

    bool m_immediate = true;
    QQueue<CommandEntity*> m_queue;
    bool m_running = false;
    QMutex m_queueMutex;

signals:
    void added(CommandEntity* entity);
    void removed(CommandEntity* entity);
    void commandStarted(CommandEntity* command);
    void commandFinished(CommandReceipt receipt);
    void runningChanged();
    void queueContentChanged();
    void immediateChanged();
};
Q_DECLARE_METATYPE(CommandQueue*)

#endif // COMMANDQUEUE_H
