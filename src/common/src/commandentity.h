#ifndef COMMANDENTITY_H
#define COMMANDENTITY_H

#include <QObject>

#include <commandentityinfo.h>

class CommandEntity : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(CommandEntityInfo info READ info CONSTANT)

public:
    explicit CommandEntity(QObject *parent = Q_NULLPTR,
                           CommandEntityInfo info = CommandEntityInfo());
    ~CommandEntity();

    enum CommandEntityState {
        IDLE,
        RUNNING,
        FINISHED,
        ABORTED
    };
    qreal progress();

    virtual bool staticProgress() const {
        // Whether the CommandEntity can update it's progress
        // depends on the implementation. Fall back to static progression.
        return true;
    }

public slots:
    void run();
    void abort(bool intended = false);
    bool isFinished() const { return m_state == FINISHED; }
    bool isRunning() const { return m_state == RUNNING; }
    bool abortIntended() const { return m_abortIntended; }
    CommandEntityState state() { return m_state; }
    const CommandEntityInfo info() const
    { return m_commandInfo; }
    const QVariantMap resultData() const
    { return isFinished() ? m_resultData : QVariantMap(); }

protected:
    virtual bool startWork() {
        if (state() == RUNNING)
            return true;

        if (state() != IDLE) {
            abortWork();
            Q_EMIT aborted();
            return false;
        }
        return true;
    }

    virtual bool abortWork() {
        if (state() == ABORTED)
            return false;
        return true;
    }

    void setProgress(qreal progress);
    void setState(const CommandEntityState& state);

    CommandEntityInfo m_commandInfo;
    QVariantMap m_resultData;

private:
    qreal m_progress = 0.0;
    CommandEntityState m_state = IDLE;
    bool m_abortIntended = false;

signals:
    void started();
    void aborted();
    void progressChanged();
    void stateChanged(CommandEntityState state);

    // Should be emitted by the implementation
    void done();
};
Q_DECLARE_METATYPE(CommandEntity*)

#endif // COMMANDENTITY_H
