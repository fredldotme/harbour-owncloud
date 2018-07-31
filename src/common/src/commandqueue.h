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

public:
    CommandReceipt() : doneTime(QDateTime::currentDateTime()), finished(false) {}
    CommandReceipt(CommandEntityInfo info, QVariant result,
                   QDateTime doneTime, bool finished) :
        info(info), result(result),
        doneTime(doneTime), finished(finished) {}

    const CommandEntityInfo info;
    const QVariant result;
    const QDateTime doneTime;
    const bool finished;
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

    bool m_immediate = false;
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
