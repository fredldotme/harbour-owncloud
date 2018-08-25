#ifndef SYNCDB_H
#define SYNCDB_H

#include <QObject>
#include <QtSql/QSqlDatabase>

class SyncDb : public QObject
{
    Q_OBJECT
public:
    explicit SyncDb(QObject *parent = Q_NULLPTR,
                    QString userName = QStringLiteral(""));
    ~SyncDb();

private:
    void createDatabase();

private:
    QSqlDatabase m_database;

};

#endif // SYNCDB_H
