#include "utaccountsdb.h"

#include <QDebug>
#include <OnlineAccountsQt/OnlineAccounts/AuthenticationData>
#include <OnlineAccountsQt/OnlineAccounts/Account>
#include <OnlineAccountsQt/OnlineAccounts/PendingCall>
#include <OnlineAccountsQt/OnlineAccounts/PendingCallWatcher>

using namespace OnlineAccounts;

UtAccountsDb::UtAccountsDb(QObject *parent) : AccountsDbInterface(parent)
{
    QStringList parts = QString::fromUtf8(qgetenv("APP_ID")).split('_');
    if (parts.count() == 3) {
        this->m_applicationId = QStringList(parts.mid(0, 2)).join('_');
    } else {
        qWarning() << "Ubuntu.OnlineAccounts: No APP_ID defined "
                      "and no applicationId given!";
        return;
    }
    this->m_manager = new Manager(this->m_applicationId, this);
    this->m_manager->waitForReady();
    QObject::connect(this->m_manager, &Manager::accountAvailable, this, [=](Account*){
        qInfo() << "Refreshing accounts list";
        refresh();
    });

    refresh();
}

UtAccountsDb::~UtAccountsDb()
{
}

void UtAccountsDb::refresh()
{
    if (!this->m_manager)
        return;

    const QString desiredServiceId = QStringLiteral("%1_nextcloud").arg(this->m_applicationId);

    qInfo() << "Available services: ";
    bool hasService = false;
    for (const Service& service : this->m_manager->availableServices()) {
        qInfo() << "\t" << service.id() << ":" << service.displayName();
        hasService = (service.id() == desiredServiceId);
        if (hasService)
            break;
    }

    if (!hasService) {
        qWarning() << "No service found, aborting account fetching.";
        return;
    }

    QVector<AccountBase*> accounts;
    for (Account* account : this->m_manager->availableAccounts(desiredServiceId)) {
        if (!account->isValid())
            continue;

        const QString hostString = account->setting("host").toString();
        if (hostString.isEmpty())
            continue;
        qInfo() << "hostString: " << hostString;

        AuthenticationData authData(account->authenticationMethod());
        PendingCall call = account->authenticate(authData);
        AuthenticationReply reply(call);
        call.waitForFinished();

        const QString username = reply.data()["Username"].toString();
        const QString password = reply.data()["Password"].toString();

        if (username.isEmpty() || password.isEmpty())
            continue;

        AccountBase* newAccount = new AccountBase();
        newAccount->setHoststring(hostString);
        newAccount->setUsername(username);
        newAccount->setPassword(password);
        accounts.append(newAccount);

        // Also connect to changed and disabled signals of the SSO Account object
        // to refresh the list appropriately
        QObject::connect(account, &Account::changed, this, &UtAccountsDb::refresh);
        QObject::connect(account, &Account::disabled, this, &UtAccountsDb::refresh);
    }
    this->m_accounts = accounts;
    emit accountsChanged();
}


QVector<AccountBase*> UtAccountsDb::accounts()
{
    return this->m_accounts;
}

QVariantList UtAccountsDb::accountVariantList()
{
    QVariantList ret;
    for (AccountBase* settings : this->accounts()) {
        ret.append(QVariant::fromValue(settings));
    }
    return ret;
}

bool UtAccountsDb::accountExists(const AccountBase* account)
{
    Q_UNUSED(account);
    return false;
}

bool UtAccountsDb::addAccount(AccountBase* account)
{
    Q_UNUSED(account);
    return false;
}

bool UtAccountsDb::updateAccount(AccountBase* account)
{
    Q_UNUSED(account);
    return false;
}

bool UtAccountsDb::removeAccount(AccountBase* account)
{
    Q_UNUSED(account);
    return false;
}
