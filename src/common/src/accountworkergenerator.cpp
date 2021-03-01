#include "accountworkergenerator.h"

#include <QDebug>

#include <util/providerutils.h>

AccountWorkerGenerator::AccountWorkerGenerator(QObject *parent) : QObject(parent)
{
    QObject::connect(this, &AccountWorkerGenerator::databaseChanged,
                     this, &AccountWorkerGenerator::generateAccountWorkers);
}

AccountWorkers* AccountWorkerGenerator::newAccount()
{
    // TODO: dynamic account type switching before saving
    //       into accounts database

    if (!this->m_defaultNewAW.get()) {
        AccountBase* newAccountSettings =
                new AccountBase(this);

        CloudStorageProvider* browserCommandQueue =
                ProviderUtils::newStorageProviderByType(this, newAccountSettings);
        CloudStorageProvider* transferCommandQueue =
                ProviderUtils::newStorageProviderByType(this, newAccountSettings);
        AccountInfoProvider* accountInfoProvider =
                ProviderUtils::newAccountInfoProviderByType(this, newAccountSettings);
        SharingProvider* sharingProvider =
                ProviderUtils::newSharingProviderByType(this, newAccountSettings);

        AccountWorkers* newDefaultWorkers =
                new AccountWorkers(Q_NULLPTR,
                                   newAccountSettings,
                                   browserCommandQueue,
                                   transferCommandQueue,
                                   accountInfoProvider,
                                   sharingProvider);

        this->m_defaultNewAW = std::unique_ptr<AccountWorkers>(newDefaultWorkers);
    }

    this->m_defaultNewAW->account()->resetSettings();

    qDebug() << Q_FUNC_INFO << this->m_defaultNewAW.get();
    return this->m_defaultNewAW.get();
}

void AccountWorkerGenerator::generateAccountWorkers()
{
    if (!this->m_database) {
        qWarning() << "Database not provided";
        return;
    }

    QVector<AccountWorkers*> newAccountWorkers;
    qDebug() << "this->m_database->accounts().length() " << this->m_database->accounts().length();

    for (AccountBase* account : this->m_database->accounts()) {
        CloudStorageProvider* browserCommandQueue =
                ProviderUtils::newStorageProviderByType(this, account);
        CloudStorageProvider* transferCommandQueue =
                ProviderUtils::newStorageProviderByType(this, account);
        AccountInfoProvider* accountInfoProvider =
                ProviderUtils::newAccountInfoProviderByType(this, account);

        if (browserCommandQueue)
            browserCommandQueue->setImmediate(true);

        AccountWorkers* workers = new AccountWorkers(this,
                                                     account,
                                                     browserCommandQueue,
                                                     transferCommandQueue,
                                                     accountInfoProvider);
        newAccountWorkers.append(workers);
    }

    cleanup();
    setAccountWorkers(newAccountWorkers);
}

void AccountWorkerGenerator::setAccountWorkers(QVector<AccountWorkers *> accountWorkers)
{
    if (this->m_accountWorkers == accountWorkers)
        return;

    this->m_accountWorkers = accountWorkers;
    Q_EMIT accountWorkersChanged();
}

void AccountWorkerGenerator::cleanup()
{
    while (!this->m_accountWorkers.empty()) {
        AccountWorkers* workers = this->m_accountWorkers.takeLast();
        if (!workers)
            continue;
        delete workers;
    }
    Q_EMIT accountWorkersChanged();
}

QVariantList AccountWorkerGenerator::accountWorkers()
{
    QVariantList accWorkers;
    for (AccountWorkers* accountWorker : this->m_accountWorkers) {
        accWorkers.push_back(QVariant::fromValue(accountWorker));
    }
    return accWorkers;
}

AccountsDbInterface* AccountWorkerGenerator::database()
{
    return this->m_database;
}

void AccountWorkerGenerator::setDatabase(AccountsDbInterface* database)
{
    if (this->m_database == database)
        return;

    if (this->m_database) {
        QObject::disconnect(this->m_database,
                            nullptr,
                            nullptr,
                            nullptr);
    }

    this->m_database = database;

    if (this->m_database) {
           QObject::connect(this->m_database, &AccountsDbInterface::accountsChanged,
                            this, &AccountWorkerGenerator::generateAccountWorkers);
    }

    Q_EMIT databaseChanged();
}
