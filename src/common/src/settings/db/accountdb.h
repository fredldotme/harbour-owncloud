#ifndef ACCOUNTDB_H
#define ACCOUNTDB_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <settings/nextcloudsettingsbase.h>
#include <QVector>
#include <QVariantList>

class AccountDb : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList accounts READ accountVariantList NOTIFY accountsChanged)

public:
    explicit AccountDb(QObject *parent = Q_NULLPTR);
    ~AccountDb();

public slots:
    QVector<AccountBase*> accounts();
    QVariantList accountVariantList();
    bool accountExists(const AccountBase* account);
    bool addAccount(AccountBase* account);
    bool updateAccount(AccountBase* account);
    bool removeAccount(AccountBase* account);

private:
    void createDatabase();
    void cleanup();
    bool insertAccountIntoDatabase(AccountBase* account);

    QSqlDatabase m_database;
    QVector<AccountBase*> m_accounts;

signals:
    void accountsChanged();
};

#endif // ACCOUNTDB_H
