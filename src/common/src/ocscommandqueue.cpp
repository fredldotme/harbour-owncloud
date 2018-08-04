#include "ocscommandqueue.h"

#include <commands/ocs/ocsuserinfocommandentity.h>

OcsCommandQueue::OcsCommandQueue(QObject *parent, NextcloudSettingsBase* settings) :
    CommandQueue(parent), m_settings(settings)
{

}

CommandEntity* OcsCommandQueue::userInfoRequest()
{
    if (!this->m_settings)
        return Q_NULLPTR;

    if (this->m_settings->providerType() != NextcloudSettingsBase::ProviderType::Nextcloud) {
        qDebug() << "User info requests are only supported on Nextcloud and ownCloud servers";
        return Q_NULLPTR;
    }

    OcsUserInfoCommandEntity* command = new OcsUserInfoCommandEntity(this,
                                                                     this->m_settings);
    enqueue(command);
    return command;
}

NextcloudSettingsBase* OcsCommandQueue::settings()
{
    return this->m_settings;
}

void OcsCommandQueue::setSettings(NextcloudSettingsBase *v)
{
    if (this->m_settings == v)
        return;

    this->m_settings = v;
    Q_EMIT settingsChanged();
}
