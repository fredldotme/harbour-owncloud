#ifndef ACCOUNTDB_H
#define ACCOUNTDB_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include "accountsdbinterface.h"

class AccountDb : public AccountsDbInterface
{
    Q_OBJECT

    Q_PROPERTY(QVariantList accounts READ accountVariantList NOTIFY accountsChangedInherited)

signals:
    // NOTIFY signal has to be defined in the same class as the property
    // see https://bugreports.qt.io/browse/QTBUG-7684
    void accountsChangedInherited();

public:
    explicit AccountDb(QObject *parent = Q_NULLPTR);
    ~AccountDb();

    Q_INVOKABLE void refresh() override;

public slots:
    QVector<AccountBase*> accounts() override;
    QVariantList accountVariantList() override;
    bool accountExists(const AccountBase* account) override;
    bool addAccount(AccountBase* account) override;
    bool updateAccount(AccountBase* account) override;
    bool removeAccount(AccountBase* account) override;

private:
    void createDatabase();
    void upgradeDatabase();
    int currentDatabaseVersion();
    void cleanup();
    bool insertAccountIntoDatabase(AccountBase* account);

    QSqlDatabase m_database;
    QVector<AccountBase*> m_accounts;
};

#endif // ACCOUNTDB_H
