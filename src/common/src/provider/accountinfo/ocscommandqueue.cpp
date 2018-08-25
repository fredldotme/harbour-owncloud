#include "ocscommandqueue.h"

#include <commands/ocs/ocsuserinfocommandentity.h>

OcsCommandQueue::OcsCommandQueue(QObject *parent, NextcloudSettingsBase* settings) :
    AccountInfoProvider(parent, settings)
{

}

CommandEntity* OcsCommandQueue::userInfoRequest()
{
    if (!this->settings())
        return Q_NULLPTR;

    if (this->settings()->providerType() != NextcloudSettingsBase::ProviderType::Nextcloud) {
        qDebug() << "User info requests are only supported on Nextcloud and ownCloud servers";
        return Q_NULLPTR;
    }

    OcsUserInfoCommandEntity* command = new OcsUserInfoCommandEntity(this,
                                                                     this->settings());
    enqueue(command);
    return command;
}
