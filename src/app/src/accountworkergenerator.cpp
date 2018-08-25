#include "accountworkergenerator.h"

#include <QDebug>

#include <util/providerutils.h>

AccountWorkerGenerator::AccountWorkerGenerator(QObject *parent) : QObject(parent)
{
    QObject::connect(this, &AccountWorkerGenerator::databaseChanged,
                     this, &AccountWorkerGenerator::generateAccountWorkers);
}

void AccountWorkerGenerator::generateAccountWorkers()
{
    if (!this->m_database) {
        qWarning() << "Database not provided";
        return;
    }

    QVector<AccountWorkers*> newAccountWorkers;
    for (NextcloudSettingsBase* account : this->m_database->accounts()) {
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
        AccountWorkers* workers = this->m_accountWorkers.takeFirst();
        if (!workers)
            continue;
        delete workers;
    }
}

QVariantList AccountWorkerGenerator::accountWorkers()
{
    QVariantList accWorkers;
    for (AccountWorkers* accountWorker : this->m_accountWorkers) {
        accWorkers.push_back(QVariant::fromValue(accountWorker));
    }
    return accWorkers;
}

AccountDb* AccountWorkerGenerator::database()
{
    return this->m_database;
}

void AccountWorkerGenerator::setDatabase(AccountDb* database)
{
    if (this->m_database == database)
        return;

    if (this->m_database) {
        QObject::disconnect(this->m_database, 0, 0, 0);
    }

    this->m_database = database;

    if (this->m_database) {
           QObject::connect(this->m_database, &AccountDb::accountsChanged,
                            this, &AccountWorkerGenerator::generateAccountWorkers);
    }

    Q_EMIT databaseChanged();
}
