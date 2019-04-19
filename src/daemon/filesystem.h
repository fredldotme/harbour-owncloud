#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QMutex>
#include <QSet>
#include <QTimer>
#include <settings/nextcloudsettingsbase.h>

class Filesystem : public QObject
{
    Q_OBJECT

    struct WatchDelay {
        QString path;
        QTimer *timer = Q_NULLPTR;
    };

public:
    Filesystem(AccountBase* account = Q_NULLPTR,
               const QString& localPath = QStringLiteral(""));

signals:
    void fileFound(QString fullPath);

public slots:
    void inhibitScan(bool inhibit);
    void triggerRescan();

private slots:
    void scan(QString dirPath, bool recursive = false);
    void prepareScan(QString dirPath);

private:
    void rescan();

    AccountBase* m_account;
    QFileSystemWatcher m_watcher;
    const QString m_localPath;
    QSet<QString> m_existingFiles;
    bool m_inhibit = false;

    /* Camera creates temporary files before writing out content.
     * This condition can be triggered by having ~/Pictures be a
     * symlink to the SD card, as the SD card write speed is slower.
     * Delay scan of respective paths with a timer. This also improves
     * situations where a user shoots multiple pics repeatedly.*/
    QList<WatchDelay> m_delayers;
    QMutex m_delayLock;

    bool isDelayActive(QString path);
    void resetDelay(QString path);
    void insertDelay(QString path);
    void clearDelays();
};

#endif//FILESYSTEM_H
