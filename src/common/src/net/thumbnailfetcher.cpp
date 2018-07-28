#include "thumbnailfetcher.h"

#include <nextcloudendpointconsts.h>
#include <commands/http/httpgetcommandentity.h>
#include <net/webdav_utils.h>

#include <QStandardPaths>

ThumbnailFetcher::ThumbnailFetcher(QObject *parent) : QObject(parent)
{
}

void ThumbnailFetcher::fetchThumbnail(QString remoteFile)
{
    if (!this->m_commandQueue) {
        qWarning() << "No command queue provided";
        return;
    }

    // Make sure to use 128x128 dimension in case of negative values
    if (width() < 0) setWidth(128);
    if (height() < 0) setHeight(128);

    const QString thumbnailPath = QStringLiteral("/%1/%2/%3/%4").arg(NEXTCLOUD_ENDPOINT_THUMBNAIL,
                                                                     QString::number(width()),
                                                                     QString::number(height()),
                                                                     remoteFile);
    const QString cacheDirectory = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    const QString cachePath = cacheDirectory + QStringLiteral("/thumbnails") + remoteFile;

    const QFileInfo cacheFile(cachePath);
    const bool isCurrent = (QDateTime::currentDateTime().addDays(-5) <
                            cacheFile.lastModified());
    if (cacheFile.exists() && isCurrent) {
        qDebug() << "Reusing existing thumbnail from cache";
        setSource(QStringLiteral("file://") + cacheFile.absoluteFilePath());
        return;
    }

    const QByteArray authorization =
            QStringLiteral("%1:%2").arg(this->m_settings->username(),
                                        this->m_settings->password()).toUtf8().toBase64();
    QMap<QByteArray, QByteArray> headers;
    headers.insert(QByteArrayLiteral("Authorization"),
                   QStringLiteral("Basic %1").arg(QString(authorization)).toUtf8());


    HttpGetCommandEntity* thumbnailDownloadCommand =
            new HttpGetCommandEntity(this->m_commandQueue,
                                     thumbnailPath,
                                     headers,
                                     this->m_settings);

    QObject::connect(thumbnailDownloadCommand, &CommandEntity::done, this, [=]() {
        setFetching(false);

        QFile thumbnail(cachePath);
        if (!thumbnail.open(QFile::ReadWrite)) {
            qWarning() << "Failed to open file for write operation";
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

    this->m_commandQueue->enqueue(thumbnailDownloadCommand);
    if (!this->m_commandQueue->isRunning())
        this->m_commandQueue->run();

    setFetching(true);
    qDebug() << "Fetching started";
}

bool ThumbnailFetcher::fetching()
{
    return this->m_fetching;
}

void ThumbnailFetcher::setFetching(bool v)
{
    if (this->m_fetching == v)
        return;

    this->m_fetching = v;
    Q_EMIT fetchingChanged();
}

QString ThumbnailFetcher::source()
{
    return this->m_source;
}

void ThumbnailFetcher::setSource(const QString &v)
{
    if (this->m_source == v)
        return;

    this->m_source = v;
    qDebug() << "new thumbnail source:" << this->m_source;
    Q_EMIT sourceChanged();
}

NextcloudSettingsBase* ThumbnailFetcher::settings()
{
    return this->m_settings;
}

void ThumbnailFetcher::setSettings(NextcloudSettingsBase* v)
{
    if (this->m_settings == v)
        return;

    this->m_settings = v;
    Q_EMIT settingsChanged();
}

CommandQueue* ThumbnailFetcher::commandQueue()
{
    return this->m_commandQueue;
}

void ThumbnailFetcher::setCommandQueue(CommandQueue* v)
{
    if (this->m_commandQueue == v)
        return;

    this->m_commandQueue = v;
    Q_EMIT settingsChanged();
}

qint16 ThumbnailFetcher::width()
{
    return this->m_width;
}

void ThumbnailFetcher::setWidth(qint16 v)
{
    if (this->m_width == v)
        return;

    this->m_width = v;
    Q_EMIT widthChanged();
}

qint16 ThumbnailFetcher::height()
{
    return this->m_height;
}

void ThumbnailFetcher::setHeight(qint16 v)
{
    if (this->m_height == v)
        return;

    this->m_height = v;
    Q_EMIT heightChanged();
}
