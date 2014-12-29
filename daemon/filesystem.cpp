#include "filesystem.h"

#include "settings.h"

Filesystem::Filesystem()
{
    connect(&m_watcher, SIGNAL(directoryChanged(QString)), SLOT(scan(QString)));
}

void Filesystem::scan(QString dirPath, bool recursive)
{
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
        QString path = entry.canonicalFilePath();
        if (entry.isDir() && recursive) {
            scan(path, recursive);
            continue;
        }

        if (m_existingFiles.contains(path)) {
            qDebug() << "existing found" << path;
            continue;
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

    scan(dirInfo.canonicalFilePath(), true);
}

void Filesystem::localPathChanged()
{
    QString newPath = Settings::instance()->localPicturesPath();

    if (!m_watcher.directories().isEmpty()) {
        m_watcher.removePaths(m_watcher.directories());
    }
    m_existingFiles.clear();

    m_localPath = newPath;
    rescan();
}
