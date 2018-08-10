#include "thumbnailfetcher.h"

#include <nextcloudendpointconsts.h>
#include <commands/http/httpgetcommandentity.h>
#include <util/webdav_utils.h>

#include <QStandardPaths>

ThumbnailFetcher::ThumbnailFetcher(QObject *parent) : AbstractFetcher(parent)
{

}

void ThumbnailFetcher::setRemoteFile(QString v)
{
    if (this->m_remoteFile == v)
        return;

    this->m_remoteFile = v;
    Q_EMIT remoteFileChanged();
}

QString ThumbnailFetcher::remoteFile()
{
    return this->m_remoteFile;
}

void ThumbnailFetcher::fetch()
{
    if (!this->commandQueue()) {
        qWarning() << "No command queue provided";
        return;
    }

    if (!this->settings()) {
        qWarning() << "No user settings provided";
        return;
    }

    if (this->settings()->providerType() != NextcloudSettingsBase::ProviderType::Nextcloud) {
        qDebug() << "Thumbnail fetching is only supported on Nextcloud and ownCloud servers";
        return;
    }

    // Make sure to use 128x128 dimension in case of negative values
    if (width() < 0) setWidth(128);
    if (height() < 0) setHeight(128);

    const QString thumbnailPath = QStringLiteral("/%1/%2/%3/%4").arg(NEXTCLOUD_ENDPOINT_THUMBNAIL,
                                                                     QString::number(width()),
                                                                     QString::number(height()),
                                                                     this->remoteFile());

    const QString identifier = QStringLiteral("/thumbnails") + this->remoteFile();


    const bool fileExists = this->cacheProvider()->cacheFileExists(identifier);
    const bool isCurrent = this->cacheProvider()->isFileCurrent(identifier);

    if (fileExists && isCurrent) {
        qDebug() << "Reusing existing thumbnail from cache";
        setSource(QStringLiteral("file://") + this->cacheProvider()->getPathForIdentifier(identifier));
        return;
    }

    HttpGetCommandEntity* thumbnailDownloadCommand =
            new HttpGetCommandEntity(this->commandQueue(),
                                     thumbnailPath,
                                     prepareOcsHeaders(this->settings()),
                                     this->settings());

    QObject::connect(thumbnailDownloadCommand, &CommandEntity::done, this, [=]() {
        setFetching(false);
        const QString cacheFilePath = this->cacheProvider()->getPathForIdentifier(identifier);

        QFile* thumbnail = this->cacheProvider()->getCacheFile(identifier, QFile::ReadWrite);
        if (!thumbnail) {
            qWarning() << "Failed to open cache identifier" << identifier << "for write operation";
            return;
        }

        const QByteArray content =
                thumbnailDownloadCommand->resultData()["content"].toByteArray();

        if (thumbnail->write(content) < 0) {
            qWarning() << "Failed to write thumbnail file";
            return;
        }
        thumbnail->flush();
        thumbnail->close();
        delete thumbnail;
        setSource(QStringLiteral("file://") + cacheFilePath);
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
