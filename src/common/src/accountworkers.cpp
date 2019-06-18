#include "accountworkers.h"

AccountWorkers::AccountWorkers() : AccountWorkers(Q_NULLPTR,
                                                  new AccountBase)
{
}

AccountWorkers::AccountWorkers(QObject *parent,
                               AccountBase* account,
                               CloudStorageProvider* browserCommandQueue,
                               CloudStorageProvider* transferCommandQueue,
                               AccountInfoProvider* accountInfoCommandQueue,
                               SharingProvider* sharingProvider) :
    QObject(parent),
    m_account(account),
    m_browserCommandQueue(browserCommandQueue),
    m_transferCommandQueue(transferCommandQueue),
    m_accountInfoCommandQueue(accountInfoCommandQueue),
    m_sharingProvider(sharingProvider)
{
    this->m_cacheProvider = new CacheProvider(this, account);
    this->m_avatarFetcher = new AvatarFetcher(this);
    this->m_avatarFetcher->setCacheProvider(this->m_cacheProvider);
    this->m_avatarFetcher->setCommandQueue(this->m_browserCommandQueue);
    this->m_thumbnailFetcher = new ThumbnailFetcher(this);
    this->m_thumbnailFetcher->setCacheProvider(this->m_cacheProvider);
    this->m_thumbnailFetcher->setCommandQueue(this->m_browserCommandQueue);
}

AccountBase* AccountWorkers::account()
{
    return this->m_account;
}

CloudStorageProvider* AccountWorkers::browserCommandQueue()
{
    return this->m_browserCommandQueue;
}

CloudStorageProvider* AccountWorkers::transferCommandQueue()
{
    return this->m_transferCommandQueue;
}

AccountInfoProvider* AccountWorkers::accountInfoCommandQueue()
{
    return this->m_accountInfoCommandQueue;
}

SharingProvider* AccountWorkers::sharingProviderCommandQueue()
{
    return this->m_sharingProvider;
}

CacheProvider* AccountWorkers::cacheProvider()
{
    return this->m_cacheProvider;
}

AvatarFetcher* AccountWorkers::avatarFetcher()
{
    return this->m_avatarFetcher;
}

ThumbnailFetcher* AccountWorkers::thumbnailFetcher()
{
    return this->m_thumbnailFetcher;
}
