#include "syncdb.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QStandardPaths>

SyncDb::SyncDb(QObject *parent, QString userName) : QObject(parent)
{
    if (!qApp) {
        qWarning() << "QCoreApplication object hasn't been instantiated yet, bailing out.";
        return;
    }

    const QString dbFilePath =
            QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
            + QStringLiteral("/%1/%2/sync.db").arg(qApp->applicationName(), userName);

    const QFileInfo dbFileInfo(dbFilePath);
    const QDir dbDir = dbFileInfo.absoluteDir();

    if (!(dbDir.exists() || dbDir.mkpath(dbDir.absolutePath()))) {
        qWarning() << "Failed to create necessary directory" << dbDir.absolutePath();
        return;
    }

    const QString dbName = QStringLiteral("sync_%1").arg(userName);
    if (QSqlDatabase::contains(dbName))
        this->m_database = QSqlDatabase::database(dbName);
    else
        this->m_database = QSqlDatabase::addDatabase("QSQLITE", dbName);

    this->m_database.setDatabaseName(dbFilePath);
    createDatabase();
}

SyncDb::~SyncDb()
{

}

void SyncDb::createDatabase()
{
    if (!this->m_database.open()) {
        qWarning() << "Failed to open SyncDb database"
                   << this->m_database.lastError().text();
        return;
    }

    const QString tangledescriptions =
            QStringLiteral("CREATE table tangledescriptions "
                           "(localPath TEXT,"
                           "remotePath TEXT,"
                           "PRIMARY KEY(localPath, remotePath));");
    const QString files =
            QStringLiteral("CREATE table files "
                           "(fileId TEXT," // used by the Nextcloud/ownCloud storage provider
                           "uniqueId TEXT,"
                           "relativePath TEXT,"
                           "PRIMARY KEY(fileId));");
    const QString version =
            QStringLiteral("CREATE table version (versionNumber INTEGER, "
                           "PRIMARY KEY(versionNumber));");
    const QString versionInsert =
            QStringLiteral("INSERT or REPLACE INTO version "
                           "values(1);");

    const QStringList existingTables = this->m_database.tables();
    qDebug() << "existing tables:" << existingTables;

    if (!existingTables.contains("version")) {
        QSqlQuery versionCreateQuery = this->m_database.exec(version);
        if (versionCreateQuery.lastError().type() != QSqlError::NoError) {
            qWarning() << "Failed to create version table, error:"
                       << versionCreateQuery.lastError().text();
            return;
        }

        QSqlQuery versionInsertQuery = this->m_database.exec(versionInsert);
        if (versionInsertQuery.lastError().type() != QSqlError::NoError) {
            qWarning() << "Failed to insert into version table, error:"
                       << versionInsertQuery.lastError().text();
            return;
        }
    }

    if (!existingTables.contains("tangledescriptions")) {
        QSqlQuery tangledescriptionsCreateQuery =
                this->m_database.exec(tangledescriptions);
        if (tangledescriptionsCreateQuery.lastError().type() != QSqlError::NoError) {
            qWarning() << "Failed to create tangledescriptions table, error:"
                       << tangledescriptionsCreateQuery.lastError().text();
            return;
        }
    }

    if (!existingTables.contains("files")) {
        QSqlQuery filesCreateQuery = this->m_database.exec(files);
        if (filesCreateQuery.lastError().type() != QSqlError::NoError) {
            qWarning() << "Failed to create files table, error:"
                       << filesCreateQuery.lastError().text();
            return;
        }
    }
}
