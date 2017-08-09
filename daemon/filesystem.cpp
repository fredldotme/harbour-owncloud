#include "filesystem.h"

#include "settings.h"
#include "uploader.h"

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

    if(!Uploader::instance()->isOnline())
        return;

    if(isDelayActive(dirPath)) {
        resetDelay(dirPath);
    } else {
        insertDelay(dirPath);
    }
}

void Filesystem::scan(QString dirPath, bool recursive)
{
    if(!Uploader::instance()->isOnline())
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

    foreach(const QFileInfo &entry, entries) {
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
    if(!Uploader::instance()->isOnline())
        return;

    QFileInfo dirInfo(m_localPath);
    if (!dirInfo.exists() || !dirInfo.isDir()) {
        qWarning() << "invalid path for watching";
    }

    scan(dirInfo.absoluteFilePath(), true);
}

void Filesystem::localPathChanged()
{
    QString newPath = Settings::instance()->localPicturesPath();

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
    m_delayLock.lock();
    for(int i = 0; i < m_delayers.count(); i++) {
        if(m_delayers.at(i).path == path) {
            m_delayers.at(i).timer->stop();
            m_delayers.at(i).timer->start();
            m_delayLock.unlock();
            return;
        }
    }
    m_delayLock.unlock();
}

void Filesystem::insertDelay(QString path)
{
    m_delayLock.lock();

    WatchDelay delay;
    delay.path = path;
    delay.timer = new QTimer(this);
    delay.timer->setInterval(3000);
    delay.timer->setSingleShot(true);
    connect(delay.timer, &QTimer::timeout, [this, path]() {
        m_delayLock.lock();
        for(int i = 0; i < m_delayers.count(); i++) {
            if(m_delayers.at(i).path == path)
                m_delayers.removeAt(i);
        }
        m_delayLock.unlock();
        scan(path);
    });
    connect(delay.timer, &QTimer::timeout, delay.timer, &QObject::deleteLater);
    m_delayers.append(delay);

    m_delayLock.unlock();
}

bool Filesystem::isDelayActive(QString path)
{
    m_delayLock.lock();
    for(int i = 0; i < m_delayers.count(); i++) {
        if(m_delayers.at(i).path == path) {
            m_delayLock.unlock();
            return true;
        }
    }
    m_delayLock.unlock();
    return false;
}

void Filesystem::clearDelays()
{
    m_delayLock.lock();
    for(int i = 0; i < m_delayers.count(); i++) {
        m_delayers.at(i).timer->stop();
        m_delayers.at(i).timer->deleteLater();
    }
    m_delayers.clear();
    m_delayLock.unlock();
}
