#include "filesystem.h"

#include "settings/nextcloudsettings.h"
//#include "uploader.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMutexLocker>

Filesystem::Filesystem()
{
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &Filesystem::prepareScan);
}

Filesystem* Filesystem::instance()
{
    static Filesystem fs;
    return &fs;
}

void Filesystem::prepareScan(QString dirPath)
{
    qDebug() << dirPath;
    if (isDelayActive(dirPath)) {
        resetDelay(dirPath);
    } else {
        insertDelay(dirPath);
    }
}

void Filesystem::scan(QString dirPath, bool recursive)
{
    if (this->m_inhibit)
        return;

    qDebug() << "scanning" << dirPath;

    QDir dir(dirPath);

    if (!m_watcher.directories().contains(dirPath)) {
        m_watcher.addPath(dirPath);
    }

    const QFileInfoList entries = dir.entryInfoList(QDir::Dirs |
                                                    QDir::Files |
                                                    QDir::NoDotAndDotDot |
                                                    QDir::Readable);

    for (const QFileInfo &entry : entries) {
        QString path = entry.absoluteFilePath();
        if (entry.isDir() && recursive) {
            scan(path, recursive);
            continue;
        }

        if (m_existingFiles.contains(path)) {
            qDebug() << "existing found" << path;
            continue;
        } else if (!m_existingFiles.contains(path) && !entry.isDir()) {
            qDebug() << "New file: " << path;
        }

        if(!entry.isDir()) {
            emit fileFound(path);
            m_existingFiles.insert(path);
        } else if (entry.isDir() && !m_watcher.directories().contains(path)){
            m_watcher.addPath(path);
        }
    }
}

void Filesystem::rescan()
{
    QFileInfo dirInfo(m_localPath);
    if (!dirInfo.exists() || !dirInfo.isDir()) {
        qWarning() << "invalid path for watching";
    }

    scan(dirInfo.absoluteFilePath(), true);
}

void Filesystem::inhibitScan(bool inhibit)
{
    this->m_inhibit = inhibit;
}

void Filesystem::triggerRescan()
{
    qDebug() << Q_FUNC_INFO;

    QString newPath = NextcloudSettings::instance()->localPicturesPath();

    if (!m_watcher.directories().isEmpty()) {
        m_watcher.removePaths(m_watcher.directories());
    }
    m_existingFiles.clear();
    clearDelays();

    m_localPath = newPath;
    rescan();
}

void Filesystem::resetDelay(QString path)
{
    QMutexLocker locker(&this->m_delayLock);
    for (int i = 0; i < m_delayers.count(); i++) {
        if(m_delayers.at(i).path == path) {
            m_delayers.at(i).timer->stop();
            m_delayers.at(i).timer->start();
            return;
        }
    }
}

void Filesystem::insertDelay(QString path)
{
    WatchDelay delay;
    delay.path = path;
    delay.timer = new QTimer(this);
    delay.timer->setInterval(3000);
    delay.timer->setSingleShot(true);
    connect(delay.timer, &QTimer::timeout, [this, path]() {
        for(int i = 0; i < m_delayers.count(); i++) {
            if(m_delayers.at(i).path == path)
                m_delayers.removeAt(i);
        }
        scan(path);
    });
    delay.timer->start();
    connect(delay.timer, &QTimer::timeout, delay.timer, &QObject::deleteLater);
    m_delayers.append(delay);
}

bool Filesystem::isDelayActive(QString path)
{
    QMutexLocker locker(&this->m_delayLock);

    for(int i = 0; i < m_delayers.count(); i++) {
        if(m_delayers.at(i).path == path) {
            return true;
        }
    }
    return false;
}

void Filesystem::clearDelays()
{
    QMutexLocker locker(&this->m_delayLock);

    for(int i = 0; i < m_delayers.count(); i++) {
        m_delayers.at(i).timer->stop();
        m_delayers.at(i).timer->deleteLater();
    }
    m_delayers.clear();
}
