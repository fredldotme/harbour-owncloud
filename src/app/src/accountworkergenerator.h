#ifndef ACCOUNTWORKERGENERATOR_H
#define ACCOUNTWORKERGENERATOR_H

#include <QObject>
#include "accountworkers.h"
#include <QVariantList>
#include "settings/db/accountdb.h"

class AccountWorkerGenerator : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList accountWorkers READ accountWorkers NOTIFY accountWorkersChanged)
    Q_PROPERTY(AccountDb* database READ database WRITE setDatabase NOTIFY databaseChanged)

public:
    explicit AccountWorkerGenerator(QObject *parent = nullptr);

    QVariantList accountWorkers();
    AccountDb* database();
    void setDatabase(AccountDb* database);

private:
    void setAccountWorkers(QVector<AccountWorkers*> accountWorkers);
    void generateAccountWorkers();
    void cleanup();
    
    AccountDb* m_database = Q_NULLPTR;
    QVector<AccountWorkers*> m_accountWorkers;

signals:
    void accountWorkersChanged();
    void databaseChanged();

};

#endif // ACCOUNTWORKERGENERATOR_H
