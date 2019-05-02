#include "filepathutil.h"

#include <QDebug>
#include <QFileInfo>
#include <QStandardPaths>

#include <nextcloudendpointconsts.h>

QString FilePathUtil::destinationFromMIME(const QString& mime)
{
    QStandardPaths::StandardLocation rootLocation;

    if (mime.startsWith("image")) {
        rootLocation = QStandardPaths::PicturesLocation;
    } else if (mime.startsWith("video")) {
        rootLocation = QStandardPaths::MoviesLocation;
    } else if (mime.startsWith("audio")) {
        rootLocation = QStandardPaths::MusicLocation;
    } else if (mime.startsWith("application")) {
        rootLocation = QStandardPaths::DocumentsLocation;
    } else {
        rootLocation = QStandardPaths::DownloadLocation;
    }

    return QStandardPaths::writableLocation(rootLocation);
}


QString FilePathUtil::destination(AccountBase* account)
{
    if (!account) {
        qWarning() << "account is NULL, bailing out.";
        return QStringLiteral("/");
    }

#ifdef GHOSTCLOUD_UBUNTU_TOUCH
    const QStandardPaths::StandardLocation location = QStandardPaths::CacheLocation;
#else
    const QStandardPaths::StandardLocation location = QStandardPaths::DownloadLocation;
#endif

    const QString pattern = QStringLiteral("%1/GhostCloud/%2/%3/%4/");
    return pattern.arg(QStandardPaths::writableLocation(location),
                       account->hostname(),
                       QString::number(account->port()),
                       account->username());
}

QString FilePathUtil::getCanonicalPath(const QString &path)
{
    const QString token = QStringLiteral("..");
    const QString slash = QStringLiteral("/");
    const QStringList dirs = path.split(slash, QString::SkipEmptyParts);

    QStringList newDirs;
    for (const QString &dir : dirs) {
        if (dir == token)
            newDirs.pop_back();
        else
            newDirs.push_back(dir);
    }

    QString ret = slash;
    if (newDirs.length() > 0)
        ret += newDirs.join(slash) + slash;

    return ret;
}

QString FilePathUtil::getWebDavFileUrl(const QString &path,
                                       AccountBase *settings)
{
    if (!settings) {
        return QStringLiteral();
    }

    const QString urlString = settings->hoststring() + "/" +
            NEXTCLOUD_ENDPOINT_WEBDAV + path;
    qDebug() << Q_FUNC_INFO << urlString;
    return urlString;
}

bool FilePathUtil::fileExists(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return !fileInfo.isDir() && fileInfo.exists();
}

bool FilePathUtil::removeFile(const QString &filePath)
{
    return QFile::remove(filePath);
}
