#ifndef ACCOUNTWORKERS_H
#define ACCOUNTWORKERS_H

#include <QObject>

#include <settings/nextcloudsettingsbase.h>
#include <provider/storage/cloudstorageprovider.h>
#include <provider/accountinfo/accountinfoprovider.h>
#include <provider/sharing/sharingprovider.h>
#include <net/avatarfetcher.h>
#include <net/thumbnailfetcher.h>
#include <cacheprovider.h>

class AccountWorkers : public QObject
{
    Q_OBJECT

    Q_PROPERTY(AccountBase* account READ account CONSTANT)
    Q_PROPERTY(CloudStorageProvider* browserCommandQueue READ browserCommandQueue CONSTANT)
    Q_PROPERTY(CloudStorageProvider* transferCommandQueue READ transferCommandQueue CONSTANT)
    Q_PROPERTY(AccountInfoProvider* accountInfoCommandQueue READ accountInfoCommandQueue CONSTANT)
    Q_PROPERTY(SharingProvider* sharingProvider READ sharingProviderCommandQueue CONSTANT)
    Q_PROPERTY(AvatarFetcher* avatarFetcher READ avatarFetcher CONSTANT)
    Q_PROPERTY(CacheProvider* cacheProvider READ cacheProvider CONSTANT)
    Q_PROPERTY(ThumbnailFetcher* thumbnailFetcher READ thumbnailFetcher CONSTANT)

public:
    explicit AccountWorkers();
    explicit AccountWorkers(QObject *parent,
                            AccountBase* account = Q_NULLPTR,
                            CloudStorageProvider* browserCommandQueue = Q_NULLPTR,
                            CloudStorageProvider* transferCommandQueue = Q_NULLPTR,
                            AccountInfoProvider* accountInfoCommandQueue = Q_NULLPTR,
                            SharingProvider* sharingProvider = Q_NULLPTR);

    AccountBase* account();
    CloudStorageProvider* browserCommandQueue();
    CloudStorageProvider* transferCommandQueue();
    AccountInfoProvider* accountInfoCommandQueue();
    SharingProvider* sharingProviderCommandQueue();
    CacheProvider* cacheProvider();
    AvatarFetcher* avatarFetcher();
    ThumbnailFetcher* thumbnailFetcher();

private:
    AccountBase* m_account = Q_NULLPTR;
    CloudStorageProvider* m_browserCommandQueue = Q_NULLPTR;
    CloudStorageProvider* m_transferCommandQueue = Q_NULLPTR;
    AccountInfoProvider* m_accountInfoCommandQueue = Q_NULLPTR;
    SharingProvider* m_sharingProvider = Q_NULLPTR;
    CacheProvider* m_cacheProvider = Q_NULLPTR;
    AvatarFetcher* m_avatarFetcher = Q_NULLPTR;
    ThumbnailFetcher* m_thumbnailFetcher = Q_NULLPTR;
};
Q_DECLARE_METATYPE(AccountWorkers*)

#endif // ACCOUNTWORKERS_H
