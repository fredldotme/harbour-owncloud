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

