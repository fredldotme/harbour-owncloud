#include "providerutils.h"
#include <QDebug>

#include <provider/storage/webdavcommandqueue.h>
#include <provider/accountinfo/ocscommandqueue.h>

CloudStorageProvider* ProviderUtils::newStorageProviderByType(QObject* parent, AccountBase *settings)
{
    CloudStorageProvider* provider = Q_NULLPTR;

    if (!settings) {
        qWarning() << "No valid settings object provided";
        return provider;
    }

    switch (settings->providerType()) {
    case AccountBase::Nextcloud:
    case AccountBase::WebDav:
        provider = new WebDavCommandQueue(parent, settings);
        break;
    default:
        qWarning() << "Unsupported providerType" << settings->providerType();
        break;
    }

    return provider;
}

AccountInfoProvider* ProviderUtils::newAccountInfoProviderByType(QObject* parent, AccountBase* settings)
{
    AccountInfoProvider* provider = Q_NULLPTR;

    if (!settings) {
        qWarning() << "No valid settings object provided";
        return provider;
    }

    switch (settings->providerType()) {
    case AccountBase::Nextcloud:
        provider = new OcsCommandQueue(parent, settings);
        break;
    default:
        qWarning() << "No AccountInfoProvider available for providerType" << settings->providerType();
        break;
    }

    return provider;
}
