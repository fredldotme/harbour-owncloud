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
    Q_INVOKABLE void refresh() override;

private:
    OnlineAccounts::Manager* m_manager = nullptr;
    QVector<AccountBase*> m_accounts;
};

#endif // UTACCOUNTSDB_H
