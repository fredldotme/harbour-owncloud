#include "commandentity.h"

#include <QDebug>

CommandEntity::CommandEntity(QObject *parent) : QObject(parent)
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
    abort();
}

qreal CommandEntity::progress()
{
    return this->m_progress;
}

void CommandEntity::setProgress(qreal progress)
{
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

void CommandEntity::abort()
{
    abortWork();
    Q_EMIT aborted();
}
