#ifndef COMMANDENTITY_H
#define COMMANDENTITY_H

#include <QObject>

#include <commandentityinfo.h>

class CommandEntity : public QObject
{
    Q_OBJECT

public:
    explicit CommandEntity(QObject *parent = nullptr);
    ~CommandEntity();

    enum CommandEntityState {
        IDLE,
        RUNNING,
        FINISHED,
        ABORTED
    };

    void run();
    void abort();
    bool isFinished() { return m_state == FINISHED; }
    bool isRunning() { return m_state == RUNNING; }
    const CommandEntityInfo info() const { return m_commandInfo; }
    CommandEntityState state() { return m_state; }

protected:
    virtual void startWork() {}
    virtual void abortWork() {}
    void setProgress(qreal progress);
    void setState(const CommandEntityState& state);

    CommandEntityInfo m_commandInfo;

private:
    qreal m_progress = 0.0;
    CommandEntityState m_state = IDLE;

signals:
    void started();
    void aborted();
    void progressChanged(qreal progress);
    void stateChanged(CommandEntityState state);

    // Should be emitted by the implementation
    void done();
};
Q_DECLARE_METATYPE(CommandEntity*)

#endif // COMMANDENTITY_H
