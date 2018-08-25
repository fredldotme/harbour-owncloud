#ifndef PROVIDERUTILS_H
#define PROVIDERUTILS_H

#include <provider/storage/cloudstorageprovider.h>
#include <provider/accountinfo/accountinfoprovider.h>

class ProviderUtils
{
public:
    static CloudStorageProvider* newStorageProviderByType(QObject* parent, NextcloudSettingsBase* settings);
    static AccountInfoProvider* newAccountInfoProviderByType(QObject* parent, NextcloudSettingsBase* settings);
};

#endif // PROVIDERUTILS_H
