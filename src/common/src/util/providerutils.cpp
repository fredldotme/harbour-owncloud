#include "providerutils.h"
#include <QDebug>

#include <provider/storage/webdavcommandqueue.h>
#include <provider/accountinfo/ocscommandqueue.h>

CloudStorageProvider* ProviderUtils::newStorageProviderByType(QObject* parent, NextcloudSettingsBase *settings)
{
    CloudStorageProvider* provider = Q_NULLPTR;

    if (!settings) {
        qWarning() << "No valid settings object provided";
        return provider;
    }

    switch (settings->providerType()) {
    case NextcloudSettingsBase::Nextcloud:
    case NextcloudSettingsBase::WebDav:
        provider = new WebDavCommandQueue(parent, settings);
        break;
    default:
        qWarning() << "Unsupported providerType" << settings->providerType();
        break;
    }

    return provider;
}

AccountInfoProvider* ProviderUtils::newAccountInfoProviderByType(QObject* parent, NextcloudSettingsBase* settings)
{
    AccountInfoProvider* provider = Q_NULLPTR;

    if (!settings) {
        qWarning() << "No valid settings object provided";
        return provider;
    }

    switch (settings->providerType()) {
    case NextcloudSettingsBase::Nextcloud:
        provider = new OcsCommandQueue(parent, settings);
        break;
    default:
        qWarning() << "No AccountInfoProvider available for providerType" << settings->providerType();
        break;
    }

    return provider;
}
