#ifndef ACCOUNTSDBINTERFACE_H
#define ACCOUNTSDBINTERFACE_H

#include <QObject>
#include <QVector>
#include <QVariantList>
#include <settings/nextcloudsettingsbase.h>

class AccountsDbInterface : public QObject
{
    Q_OBJECT

public:
    explicit AccountsDbInterface(QObject* parent = nullptr) :
        QObject(parent) {}

    virtual Q_INVOKABLE void refresh() {}

public slots:
    virtual QVector<AccountBase*> accounts() {
        return QVector<AccountBase*>();
    }

    virtual QVariantList accountVariantList() {
        return QVariantList();
    }

    virtual bool accountExists(const AccountBase* account) {
        Q_UNUSED(account);
        return false;
    }

    virtual bool addAccount(AccountBase* account) {
        Q_UNUSED(account);
        return false;
    }

    virtual bool updateAccount(AccountBase* account) {
        Q_UNUSED(account);
        return false;
    }

    virtual bool removeAccount(AccountBase* account) {
        Q_UNUSED(account);
        return false;
    }

signals:
    void accountsChanged();
};

#endif // ACCOUNTSDBINTERFACE_H
