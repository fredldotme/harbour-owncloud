#ifndef COMMANDENTITY_H
#define COMMANDENTITY_H

#include <QObject>

#include <commandentityinfo.h>

class CommandEntity : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

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
    bool isFinished() const { return m_state == FINISHED; }
    bool isRunning() const { return m_state == RUNNING; }
    CommandEntityState state() { return m_state; }

public slots:
    const CommandEntityInfo info() const
    { return m_commandInfo; }
    const QVariant resultData() const
    { return isFinished() ? m_resultData : QVariant(); }

protected:
    virtual void startWork() {}
    virtual void abortWork() {}
    qreal progress();
    void setProgress(qreal progress);
    void setState(const CommandEntityState& state);

    CommandEntityInfo m_commandInfo;
    QVariant m_resultData;

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
