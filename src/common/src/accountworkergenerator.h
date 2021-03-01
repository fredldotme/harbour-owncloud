#ifndef ACCOUNTWORKERGENERATOR_H
#define ACCOUNTWORKERGENERATOR_H

#include <QObject>
#include "accountworkers.h"
#include <QVariantList>
#include "settings/db/accountdb.h"
#include <memory>

class AccountWorkerGenerator : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList accountWorkers READ accountWorkers NOTIFY accountWorkersChanged)
    Q_PROPERTY(AccountsDbInterface* database READ database WRITE setDatabase NOTIFY databaseChanged)

public:
    explicit AccountWorkerGenerator(QObject *parent = nullptr);

    QVariantList accountWorkers();
    AccountsDbInterface* database();
    void setDatabase(AccountsDbInterface* database);

public slots:
    AccountWorkers* newAccount();

private:
    void setAccountWorkers(QVector<AccountWorkers*> accountWorkers);
    void generateAccountWorkers();
    void cleanup();
    
    AccountsDbInterface* m_database = Q_NULLPTR;
    QVector<AccountWorkers*> m_accountWorkers;
    std::unique_ptr<AccountWorkers> m_defaultNewAW;

signals:
    void accountWorkersChanged();
    void databaseChanged();

};

#endif // ACCOUNTWORKERGENERATOR_H
