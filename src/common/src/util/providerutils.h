#ifndef PROVIDERUTILS_H
#define PROVIDERUTILS_H

#include <provider/storage/cloudstorageprovider.h>
#include <provider/accountinfo/accountinfoprovider.h>

class ProviderUtils
{
public:
    static CloudStorageProvider* newStorageProviderByType(QObject* parent, AccountBase* settings);
    static AccountInfoProvider* newAccountInfoProviderByType(QObject* parent, AccountBase* settings);
};

#endif // PROVIDERUTILS_H
