#include "ocscommandqueue.h"

#include <commands/ocs/ocsuserinfocommandentity.h>

#include <QVariantMap>

OcsCommandQueue::OcsCommandQueue(QObject *parent, AccountBase* settings) :
    AccountInfoProvider(parent, settings)
{
    connect(this, &OcsCommandQueue::commandFinished,
            this, [=](CommandReceipt receipt) {
        const bool enabled = receipt.result.toMap()["enabled"].toBool();
        qDebug() << "receipt.result: " << receipt.result;
        qDebug() << "Enabled? " << enabled;

        if (!enabled)
            return;

        Q_EMIT userInfoChanged();
    });
}

CommandEntity* OcsCommandQueue::userInfoRequest()
{
    if (!this->settings())
        return Q_NULLPTR;

    if (this->settings()->providerType() != AccountBase::ProviderType::Nextcloud) {
        qDebug() << "User info requests are only supported on Nextcloud and ownCloud servers";
        return Q_NULLPTR;
    }

    OcsUserInfoCommandEntity* command = new OcsUserInfoCommandEntity(this,
                                                                     this->settings());
    enqueue(command);
    return command;
}

QString OcsCommandQueue::providerSettingsUrl()
{
    if (!this->settings())
        return QString();

    if (this->settings()->providerType() != AccountBase::ProviderType::Nextcloud) {
        qDebug() << "Provider settings are only supported on Nextcloud and ownCloud servers";
        return Q_NULLPTR;
    }

    // TODO: correct URL to settings landing page
    // TODO: split implementation into NC and oC
    return QString();
}
