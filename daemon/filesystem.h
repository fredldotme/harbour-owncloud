#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QMutex>
#include <QSet>
#include <QTimer>

class Filesystem : public QObject
{
    Q_OBJECT

    struct WatchDelay {
        QString path;
        QTimer *timer;
    };

public:
    Filesystem();

signals:
    void fileFound(QString fullPath);

private slots:
    void localPathChanged();
    void scan(QString dirPath, bool recursive = false);
    void prepareScan(QString dirPath);

private:
    void rescan();
    bool isDelayActive(QString path);
    void resetDelay(QString path);
    void insertDelay(QString path);

    QFileSystemWatcher m_watcher;
    QString m_localPath;
    QSet<QString> m_existingFiles;

    /* Camera creates temporary files before writing out content.
     * This condition can be triggered by having ~/Pictures be a
     * symlink to the SD card, as the SD card write speed is slower.
     * Delay scan of respective paths with a timer. This also improves
     * situations where a user shoots multiple pics repeatedly.*/
    QList<WatchDelay> m_delayers;
    QMutex m_delayLock;
};

#endif//FILESYSTEM_H
