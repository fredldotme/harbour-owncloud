#include "accountdb.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QVariant>

const int MAX_CURRENT_DB_VERSION = 1;

AccountDb::AccountDb(QObject *parent) : AccountsDbInterface(parent)
{
    if (!qApp) {
        qWarning() << "QCoreApplication object hasn't been instantiated yet, bailing out.";
        return;
    }

#ifdef GHOSTCLOUD_UBUNTU_TOUCH
    const QStandardPaths::StandardLocation location = QStandardPaths::AppConfigLocation;
#else
    const QStandardPaths::StandardLocation location = QStandardPaths::ConfigLocation;
#endif

    const QString dbFilePath =
            QStandardPaths::writableLocation(location)
            + QStringLiteral("/%1/accounts.db").arg(qApp->applicationName());
    const QString dbName = QStringLiteral("accountdb");

    const QFileInfo dbFileInfo(dbFilePath);
    const QDir dbDir = dbFileInfo.absoluteDir();

    if (!(dbDir.exists() || dbDir.mkpath(dbDir.absolutePath()))) {
        qWarning() << "Failed to create necessary directory" << dbDir.absolutePath();
        return;
    }

    if (QSqlDatabase::contains(dbName))
        this->m_database = QSqlDatabase::database(dbName);
    else
        this->m_database = QSqlDatabase::addDatabase("QSQLITE", dbName);

    qDebug() << "Database location: " << dbFilePath;
    this->m_database.setDatabaseName(dbFilePath);
    createDatabase();
    upgradeDatabase();
    refresh();
}

AccountDb::~AccountDb()
{
    cleanup();
}

void AccountDb::createDatabase()
{
    if (!this->m_database.open()) {
        qWarning() << "Failed to open AccountDb database:"
                   << this->m_database.lastError().text();
        return;
    }

    const QString version =
            QStringLiteral("CREATE table version (versionNumber INTEGER, "
                           "PRIMARY KEY(versionNumber));");
    const QString versionInsert =
            QStringLiteral("INSERT or REPLACE INTO version "
                           "values(1);");
    const QString accounts =
            QStringLiteral("CREATE table accounts ("
                           "username TEXT,"
                           "password TEXT,"
                           "hoststring TEXT,"
                           "autologin INTEGER,"
                           "notifications INTEGER,"
                           "providerType INTEGER,"
                           "uploadAutomatically INTEGER,"
                           "mobileUpload INTEGER,"
                           "certMd5 TEXT,"
                           "certSha1 TEXT,"
                           "PRIMARY KEY(hoststring, username, providerType));");

    const QStringList existingTables = this->m_database.tables();
    qDebug() << "existing tables:" << existingTables;

    if (!existingTables.contains("version")) {
        const QSqlQuery versionCreateQuery = this->m_database.exec(version);
        if (versionCreateQuery.lastError().type() != QSqlError::NoError) {
            qWarning() << "Failed to create version table, error:"
                       << versionCreateQuery.lastError().text();
            return;
        }

        const QSqlQuery versionInsertQuery = this->m_database.exec(versionInsert);
        if (versionInsertQuery.lastError().type() != QSqlError::NoError) {
            qWarning() << "Failed to create version table, error:"
                       << versionInsertQuery.lastError().text();
            return;
        }
    }

    if (!existingTables.contains("accounts")) {
        const QSqlQuery accountsCreateQuery = this->m_database.exec(accounts);
        if (accountsCreateQuery.lastError().type() != QSqlError::NoError) {
            qWarning() << "Failed to create version table, error:"
                       << accountsCreateQuery.lastError().text();
            return;
        }
    }
}

int AccountDb::currentDatabaseVersion()
{
    const QString statement =
            QStringLiteral("SELECT versionNumber from VERSION;");
    QSqlQuery query = this->m_database.exec(statement);

    // There's supposed to be only one row in the table
    if (query.first()) {
        bool parseOk = false;
        const QString columnName = QStringLiteral("versionNumber");
        const int currentVersion =
                query.value(columnName).toInt(&parseOk);

        if (parseOk)
            return currentVersion;
    }

    return -1;
}

void AccountDb::upgradeDatabase()
{
    const int currentDbVersion = currentDatabaseVersion();
    qDebug() << Q_FUNC_INFO << "current version"
             << currentDbVersion;

    if (MAX_CURRENT_DB_VERSION == currentDbVersion)
        return;

    qInfo() << "Upgrading database tables";
    // TODO: New tables, columns, etc.
}

void AccountDb::cleanup()
{
    while (!this->m_accounts.empty()) {
        AccountBase* account = this->m_accounts.takeFirst();
        if (!account)
            continue;
        delete account;
    }
}

void AccountDb::refresh()
{
    accounts();
    Q_EMIT accountsChanged();
}

QVector<AccountBase*> AccountDb::accounts()
{
    QVector<AccountBase*> tmpAccounts;

    if (!this->m_database.open()) {
        qWarning() << "Failed to open AccountDb database:"
                   << this->m_database.lastError().text();
        return tmpAccounts;
    }

    const QString queryString =
            QStringLiteral("SELECT username, password, hoststring, autologin,"
                           "notifications, providerType, uploadAutomatically,"
                           "mobileUpload, certMd5, certSha1 from accounts;");

    QSqlQuery selectQuery = this->m_database.exec(queryString);
    if (selectQuery.lastError().type() != QSqlError::NoError) {
        qWarning() << "Failed to read accounts from database:"
                   << selectQuery.lastError().text();
        return tmpAccounts;
    }

    while (selectQuery.next()) {
        const QString username = selectQuery.value("username").toString();
        const QString password = selectQuery.value("password").toString();
        const QString hoststring = selectQuery.value("hoststring").toString();
        const bool autologin = selectQuery.value("autologin").toBool();
        const bool notifications = selectQuery.value("notifications").toBool();
        const int providerType = selectQuery.value("providerType").toInt();
        const bool uploadAutomatically = selectQuery.value("uploadAutomatically").toBool();
        const bool mobileUpload = selectQuery.value("mobileUpload").toBool();
        const QString certMd5 = selectQuery.value("certMd5").toString();
        const QString certSha1 = selectQuery.value("certSha1").toString();

        AccountBase* account = new AccountBase(this);
        account->setUsername(username);
        account->setPassword(password);
        account->setHoststring(hoststring);
        account->setAutoLogin(autologin);
        account->setNotifications(notifications);
        account->setProviderType(providerType);
        account->setUploadAutomatically(uploadAutomatically);
        account->setMobileUpload(mobileUpload);
        account->setMd5Hex(certMd5);
        account->setSha1Hex(certSha1);
        tmpAccounts.push_back(account);
    }

    cleanup();
    this->m_accounts = tmpAccounts;
    return this->m_accounts;
}

QVariantList AccountDb::accountVariantList()
{
    QVariantList ret;
    for (AccountBase* settings : this->accounts()) {
        ret.append(QVariant::fromValue(settings));
    }
    return ret;
}

bool AccountDb::accountExists(const AccountBase *account)
{
    if (!account) {
        qWarning() << "Invalid account provided, bailing out";
        return false;
    }

    if (!this->m_database.open()) {
        qWarning() << "Failed to open AccountDb database:"
                   << this->m_database.lastError().text();
        return false;
    }

    const QString existanceCheck =
            QStringLiteral("SELECT hoststring, username, providerType from accounts "
                           "WHERE hoststring=':hoststring' "
                           "AND username=':username' "
                           "AND providerType=':providerType'");

    QSqlQuery existanceQuery(this->m_database);
    existanceQuery.prepare(existanceCheck);
    existanceQuery.bindValue(":hoststring", QVariant(account->hoststring()));
    existanceQuery.bindValue(":username", QVariant(account->username()));
    existanceQuery.bindValue(":providerType", QVariant(account->providerType()));

    const bool existanceSuccess = existanceQuery.exec();
    if (!existanceSuccess) {
        qWarning() << "Failed to query for existing user account"
                   << account->username() << ", provider"
                   << account->providerType() << ", error:"
                   << existanceQuery.lastError().text();
        return false;
    }

    return existanceQuery.record().count() > 0;
}

bool AccountDb::insertAccountIntoDatabase(AccountBase *account)
{
    if (!account) {
        qWarning() << "Invalid account provided, bailing out";
        return false;
    }

    const QString accountInsert =
            QStringLiteral("INSERT or REPLACE INTO accounts"
                           " (username, password, hoststring, autologin,"
                           " notifications, providerType, uploadAutomatically,"
                           " mobileUpload, certMd5, certSha1)"
                           "values("
                           " :username, :password, :hoststring, :autologin,"
                           " :notifcations, :providerType, :uploadAutomatically,"
                           " :mobileUpload, :certMD5, :certSHA1);");

    const QString placeHolderPattern = QStringLiteral(":%1");

    QSqlQuery accountInsertQuery(this->m_database);
    accountInsertQuery.prepare(accountInsert);
    accountInsertQuery.bindValue(placeHolderPattern.arg(NEXTCLOUD_SETTINGS_KEY_USERNAME),
                                 account->username());
    accountInsertQuery.bindValue(placeHolderPattern.arg(NEXTCLOUD_SETTINGS_KEY_PASSWORD),
                                 account->password());
    accountInsertQuery.bindValue(placeHolderPattern.arg(NEXTCLOUD_SETTINGS_KEY_HOSTSTRING),
                                 account->hoststring());
    accountInsertQuery.bindValue(placeHolderPattern.arg(NEXTCLOUD_SETTINGS_KEY_AUTOLOGIN),
                                 account->autoLogin());
    accountInsertQuery.bindValue(placeHolderPattern.arg(NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS),
                                 account->notifications());
    accountInsertQuery.bindValue(placeHolderPattern.arg(NEXTCLOUD_SETTINGS_KEY_PROVIDERTYPE),
                                 account->providerType());
    accountInsertQuery.bindValue(placeHolderPattern.arg(NEXTCLOUD_SETTINGS_KEY_UPLOADAUTOMATICALLY),
                                 account->uploadAutomatically());
    accountInsertQuery.bindValue(placeHolderPattern.arg(NEXTCLOUD_SETTINGS_KEY_MOBILEUPLOAD),
                                 account->mobileUpload());
    accountInsertQuery.bindValue(placeHolderPattern.arg(NEXTCLOUD_SETTINGS_KEY_CERTMD5),
                                 account->md5Hex());
    accountInsertQuery.bindValue(placeHolderPattern.arg(NEXTCLOUD_SETTINGS_KEY_CERTSHA1),
                                 account->sha1Hex());

    const bool insertSuccess = accountInsertQuery.exec();
    if (!insertSuccess) {
        qWarning() << "Failed to insert user account" << account->username()
                   << ", error:" << accountInsertQuery.lastError().text();
        return false;
    }

    return true;
}

bool AccountDb::addAccount(AccountBase *account)
{
    if (!account) {
        qWarning() << "Invalid account provided, bailing out";
        return false;
    }

    if (this->m_accounts.contains(account)) {
        qWarning() << "Provided account is already in the list of loaded accounts, "
                   << "use updateAccount() instead. Bailing out.";
        return false;
    }

    const bool insertionSuccess = insertAccountIntoDatabase(account);
    if (insertionSuccess) {
        QObject::connect(account, &AccountBase::settingsChanged,
                         this, [this, account](){
            this->updateAccount(account);
        });
        Q_EMIT accountsChanged();
    }
    return insertionSuccess;
}

bool AccountDb::updateAccount(AccountBase *account)
{
    if (!account) {
        qWarning() << "Invalid account provided, bailing out";
        return false;
    }

    if (!this->m_accounts.contains(account)) {
        qWarning() << "Provided account is not in the list of loaded accounts";
        return false;
    }

    const bool insertSuccess = insertAccountIntoDatabase(account);
    Q_EMIT accountsChanged();
    return insertSuccess;
}

bool AccountDb::removeAccount(AccountBase *account)
{
    if (!account) {
        qWarning() << "Invalid account provided, bailing out";
        return false;
    }

    if (!this->m_accounts.contains(account)) {
        qWarning() << "Provided account is not in the list of loaded accounts";
        return false;
    }

    const QString deleteString =
            QStringLiteral("DELETE from accounts"
                           " WHERE hoststring=:hoststring"
                           " AND username=:username"
                           " AND providerType=:providerType");

    QSqlQuery deleteQuery(this->m_database);
    deleteQuery.prepare(deleteString);
    deleteQuery.bindValue(":hoststring", QVariant(account->hoststring()));
    deleteQuery.bindValue(":username", QVariant(account->username()));
    deleteQuery.bindValue(":providerType", QVariant(account->providerType()));

    const bool deleteSuccess = deleteQuery.exec();
    if (!deleteSuccess) {
        qWarning() << "Failed to delete user account"
                   << account->username() << ", provider"
                   << account->providerType() << ", error:"
                   << deleteQuery.lastError().text();
        return false;
    }

    if (this->m_accounts.contains(account)) {
        this->m_accounts.removeAll(account);
    }

    Q_EMIT accountsChanged();

    qDebug() << deleteQuery.executedQuery();

    return true;
}
