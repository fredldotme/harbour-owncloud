#include "settingsbackedcommandqueue.h"

SettingsBackedCommandQueue::SettingsBackedCommandQueue(QObject *parent, AccountBase* settings) :
    CommandQueue(parent)
{
    setSettings(settings);
}

AccountBase* SettingsBackedCommandQueue::settings()
{
    return this->m_settings;
}

void SettingsBackedCommandQueue::setSettings(AccountBase *v)
{
    if (this->m_settings == v)
        return;

    // Disconnect from current settings object if it exists
    if (this->m_settings)
        QObject::disconnect(this->m_settings, 0, 0, 0);

    this->m_settings = v;
    Q_EMIT settingsChanged();
}
