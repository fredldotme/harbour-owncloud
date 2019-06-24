#include "cacheprovider.h"

#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QStandardPaths>

#include <util/filepathutil.h>

CacheProvider::CacheProvider(QObject* parent, AccountBase* account) :
    QObject(parent)
{
    if (!account) {
        qWarning() << "No account provided, bailing out...";
        return;
    }
    const QString cacheSubdir =
            QStringLiteral("/%1/%2/%3/").arg(account->hostname(),
                                             QString::number(account->port()),
                                             account->username());
    this->m_cacheDir =
            QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
            cacheSubdir;
    this->m_downloadDir =
            FilePathUtil::destination(account);

    const int clearInterval = 1000 * 60 * 60;

    this->m_clearTimer.setInterval(clearInterval);
    this->m_clearTimer.setSingleShot(false);
    QObject::connect(&this->m_clearTimer, &QTimer::timeout, this, &CacheProvider::clearCache);
}

bool CacheProvider::cacheFileExists(const QString &identifier)
{
    const QString filePath = getPathForIdentifier(identifier);
    QFileInfo fileInfo(filePath);
    return fileInfo.exists();
}

bool CacheProvider::isFileCurrent(const QString &identifier)
{
    if (!cacheFileExists(identifier))
        return false;

    const QString filePath = getPathForIdentifier(identifier);
    QFileInfo fileInfo(filePath);
    const bool isCurrent = (QDateTime::currentDateTime().addDays(-5) <
                            fileInfo.lastModified());
    return isCurrent;
}

QFile* CacheProvider::getCacheFile(const QString &identifier, QFile::OpenMode mode)
{
    const QString filePath = getPathForIdentifier(identifier);
    const QFileInfo fileInfo(filePath);

    QDir cacheDir = fileInfo.absoluteDir();
    if (!cacheDir.exists()) {
        const QString targetCacheDir = cacheDir.absolutePath();
        if (!cacheDir.mkpath(targetCacheDir)) {
            qWarning() << "Failed to create cache directory" << targetCacheDir;
            return Q_NULLPTR;
        }
    }

    QFile* cacheFile = new QFile(filePath, this);
    if (!cacheFile->open(mode)) {
        qWarning() << "Failed to open" << filePath << "in mode" << mode;
        return Q_NULLPTR;
    }
    return cacheFile;
}

bool CacheProvider::clearPath(const QString& path)
{
    bool cacheFileRemoved = false;

    QDirIterator dirIterator(path, QDirIterator::Subdirectories);
    while(dirIterator.hasNext()) {
        const QString cacheFilePath = dirIterator.next();
        const QFileInfo cacheFile(cacheFilePath);
        if (!cacheFile.isFile())
            continue;

        if (isFileCurrent(cacheFilePath))
            continue;

        const bool removeSuccess = QFile::remove(cacheFilePath);
        if (!removeSuccess) {
            qWarning() << "Failed to remove" << cacheFilePath;
        }
        cacheFileRemoved = true;
    }

    return cacheFileRemoved;
}

void CacheProvider::clearCache()
{
    const bool cacheFileRemoved = clearPath(this->m_cacheDir);

    if (cacheFileRemoved)
        Q_EMIT cacheCleared();
}

void CacheProvider::clearDownloads()
{
    const bool cacheFileRemoved = clearPath(this->m_downloadDir);

    if (cacheFileRemoved)
        Q_EMIT cacheCleared();
}

QString CacheProvider::getPathForIdentifier(const QString &identifier)
{
    const QString slash = QStringLiteral("/");
    return this->m_cacheDir + (!identifier.startsWith(slash) ? slash : QStringLiteral("")) + identifier;
}
