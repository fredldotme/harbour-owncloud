#include "filesystem.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QSet>

Filesystem::Filesystem(AccountBase* account) :
    m_account(account)
{
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &Filesystem::prepareScan);
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

void Filesystem::scan(WatchedLocation location, QString dirPath, bool recursive)
{
    if (this->m_inhibit)
        return;

    qDebug() << "scanning" << dirPath;

    QDir dir(dirPath);

    if (!m_watcher.directories().contains(dirPath)) {
        m_watcher.addPath(dirPath);
        m_watcherLocations[dirPath]=location;
    }

    const QFileInfoList entries = dir.entryInfoList(QDir::Dirs |
                                                    QDir::Files |
                                                    QDir::NoDotAndDotDot |
                                                    QDir::Readable);

    for (const QFileInfo &entry : entries) {
        QString path = entry.absoluteFilePath();
        if (entry.isDir() && recursive) {
            scan(location, path, recursive);
            continue;
        }

        if (m_existingFiles.contains(path)) {
            qDebug() << "existing found" << path;
            continue;
        } else if (!m_existingFiles.contains(path) && !entry.isDir()) {
            qDebug() << "New file: " << path;
        }

        if(!entry.isDir()) {
            emit fileFound(location.localDir, location.name, path);
            m_existingFiles.insert(path);
        } else if (entry.isDir() && !m_watcher.directories().contains(path)){
            m_watcher.addPath(path);
            m_watcherLocations[dirPath]=location;
        }
    }
}

void Filesystem::rescan()
{
    QList<WatchedLocation> pictureLocations;

    QString internalPictureStorage = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (QFileInfo(internalPictureStorage).exists()) {
        pictureLocations << WatchedLocation{internalPictureStorage, "Internal"};
    }

    // iterate through mount volumes and try to find external drives with Pictures
    // TODO: use card UUID and some kind of persistent configuration to proper support of multiple SD cards
    int sdCardNum = 0;
    for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()){
        QString mountPoint = storage.rootPath();
        QString pictureDir = mountPoint + QDir::separator() + "Pictures";

        if (storage.isValid() &&
            storage.isReady() &&
            mountPoint.startsWith("/run/media/") && // Sailfish OS specific mount point base for SD cards!
            QDir(pictureDir).exists()){

            qDebug() << "Found photo storage:" << pictureDir;
            QString locationName = "SD Card" + (sdCardNum == 0 ? QString(): " " + QString::number(sdCardNum));
            pictureLocations << WatchedLocation{pictureDir, locationName};
            sdCardNum++;
        }
    }

    for (const WatchedLocation &location: pictureLocations) {
        QFileInfo dirInfo(location.localDir);
        if (!dirInfo.exists() || !dirInfo.isDir()) {
            qWarning() << "invalid path for watching" << dirInfo.absolutePath();
        }

        emit locationFound(location.localDir, location.name);
        scan(location, dirInfo.absoluteFilePath(), true);
    }
}

void Filesystem::inhibitScan(bool inhibit)
{
    this->m_inhibit = inhibit;
}

void Filesystem::triggerRescan()
{
    qDebug() << Q_FUNC_INFO;

    if (!this->m_account)
        return;

    if (!m_watcher.directories().isEmpty()) {
        m_watcherLocations.clear();
        m_watcher.removePaths(m_watcher.directories());
    }
    m_existingFiles.clear();
    clearDelays();

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
        if (m_watcherLocations.contains(path)) {
            scan(m_watcherLocations[path], path);
        }else{
            qWarning() << "Can't found location for path" << path;
        }
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
