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
    QVector<NextcloudSettingsBase*> accounts();
    QVariantList accountVariantList();
    bool accountExists(const NextcloudSettingsBase* account);
    bool addAccount(NextcloudSettingsBase* account);
    bool updateAccount(NextcloudSettingsBase* account);
    bool removeAccount(NextcloudSettingsBase* account);

private:
    void createDatabase();
    void cleanup();
    bool insertAccountIntoDatabase(NextcloudSettingsBase* account);

    QSqlDatabase m_database;
    QVector<NextcloudSettingsBase*> m_accounts;

signals:
    void accountsChanged();
};

#endif // ACCOUNTDB_H
