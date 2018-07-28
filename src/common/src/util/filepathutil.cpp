#include "filepathutil.h"

#include <QStandardPaths>

QString FilePathUtil::destinationFromMIME(QString mime)
{
    QStandardPaths::StandardLocation location;

    if (mime.startsWith("image")) {
        location = QStandardPaths::PicturesLocation;
    } else if (mime.startsWith("video")) {
        location = QStandardPaths::MoviesLocation;
    } else if (mime.startsWith("audio")) {
        location = QStandardPaths::MusicLocation;
    } else if (mime.startsWith("application")) {
        location = QStandardPaths::DocumentsLocation;
    } else {
        location = QStandardPaths::DownloadLocation;
    }

    return QStandardPaths::writableLocation(location);
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
