#ifndef UTACCOUNTSDB_H
#define UTACCOUNTSDB_H

#include <QObject>
#include "accountsdbinterface.h"
#include <OnlineAccountsQt/OnlineAccounts/Manager>

class UtAccountsDb : public AccountsDbInterface
{
    Q_OBJECT

    Q_PROPERTY(QVariantList accounts READ accountVariantList NOTIFY accountsChanged)

public:
    explicit UtAccountsDb(QObject *parent = nullptr);
    ~UtAccountsDb();
    Q_INVOKABLE void refresh() override;

public slots:
    QVector<AccountBase*> accounts() override;
    QVariantList accountVariantList() override;
    bool accountExists(const AccountBase* account) override;
    bool addAccount(AccountBase* account) override;
    bool updateAccount(AccountBase* account) override;
    bool removeAccount(AccountBase* account) override;

private:
    OnlineAccounts::Manager* m_manager = nullptr;
    QVector<AccountBase*> m_accounts;
};

#endif // UTACCOUNTSDB_H
