#include "commandentity.h"

#include <QDebug>
#include <QTimer>

CommandEntity::CommandEntity(QObject *parent, CommandEntityInfo info) :
    QObject(parent), m_commandInfo(info)
{
    QObject::connect(this, &CommandEntity::done, this, [=]() {
        setState(FINISHED);
    }, Qt::DirectConnection);

    QObject::connect(this, &CommandEntity::aborted, this, [=]() {
        setState(ABORTED);
    }, Qt::DirectConnection);
}

CommandEntity::~CommandEntity()
{
    qDebug() << Q_FUNC_INFO;
}

qreal CommandEntity::progress()
{
    return this->m_progress;
}

void CommandEntity::setProgress(qreal progress)
{
    // Avoid setting a progress value when the CommandEntity has static progress
    if (this->staticProgress())
        return;

    if (this->m_progress == progress)
        return;

    this->m_progress = progress;
    Q_EMIT progressChanged();
}

void CommandEntity::setState(const CommandEntityState &state)
{
    if (this->m_state == state)
        return;

    this->m_state = state;
    Q_EMIT stateChanged(this->m_state);
}

void CommandEntity::run()
{
    if (this->isRunning())
        return;

    startWork();
    Q_EMIT started();
}

void CommandEntity::abort(bool intended)
{
    qDebug() << Q_FUNC_INFO;
    QTimer::singleShot(0, this, [=](){
        this->m_abortIntended = intended;
        abortWork();
        Q_EMIT aborted();
    });
}
