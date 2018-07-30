#include "avatarfetcher.h"

#include <nextcloudendpointconsts.h>

#include <commands/http/httpgetcommandentity.h>
#include <net/webdav_utils.h>

AvatarFetcher::AvatarFetcher(QObject *parent) : AbstractFetcher(parent)
{

}

void AvatarFetcher::fetch()
{
    if (!this->commandQueue()) {
        qWarning() << "No command queue provided";
        return;
    }

    if (!this->settings()) {
        qWarning() << "No user settings provided";
        return;
    }

    // Make sure to use 128x128 dimension in case of negative values
    if (width() < 0) setWidth(128);
    if (height() < 0) setHeight(128);

    const QString thumbnailPath = NEXTCLOUD_ENDPOINT_AVATAR.arg(this->settings()->username(),
                                                                width());


    const QString cacheDirectory = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    const QString cachePath = cacheDirectory + QStringLiteral("/avatar.png");

    const QFileInfo cacheFile(cachePath);
    const QString targetCacheDir = cacheFile.absolutePath();
    const bool isCurrent = (QDateTime::currentDateTime().addDays(-5) <
                            cacheFile.lastModified());
    if (cacheFile.exists() && isCurrent) {
        qDebug() << "Reusing existing thumbnail from cache";
        setSource(QStringLiteral("file://") + cacheFile.absoluteFilePath());
        return;
    }

    HttpGetCommandEntity* thumbnailDownloadCommand =
            new HttpGetCommandEntity(this->commandQueue(),
                                     thumbnailPath,
                                     prepareOcsHeaders(this->settings()),
                                     this->settings());

    QObject::connect(thumbnailDownloadCommand, &CommandEntity::done, this, [=]() {
        setFetching(false);

        QFile thumbnail(cachePath);
        QDir cacheDir(targetCacheDir);
        if (!cacheDir.exists()) {
            if (!cacheDir.mkpath(targetCacheDir)) {
                qWarning() << "Failed to create cache directory" << targetCacheDir;
                return;
            }
        }

        if (!thumbnail.open(QFile::ReadWrite)) {
            qWarning() << "Failed to open" << cachePath << "for write operation";
            return;
        }

        const QByteArray content =
                thumbnailDownloadCommand->resultData().toMap()["content"].toByteArray();

        if (thumbnail.write(content) < 0) {
            qWarning() << "Failed to write thumbnail file";
            return;
        }
        thumbnail.close();
        setSource(QStringLiteral("file://") + cachePath);
    });
    QObject::connect(thumbnailDownloadCommand, &CommandEntity::aborted, this, [=]() {
        setFetching(false);
    });

    this->commandQueue()->enqueue(thumbnailDownloadCommand);
    if (!this->commandQueue()->isRunning())
        this->commandQueue()->run();

    setFetching(true);
    qDebug() << "Fetching started";
}
