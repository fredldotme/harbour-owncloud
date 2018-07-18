#include "thumbnailfetcher.h"

#include <commands/filedownloadcommandentity.h>
#include <net/webdav_utils.h>

#include <QStandardPaths>

ThumbnailFetcher::ThumbnailFetcher(QObject *parent) : QObject(parent)
{
}

void ThumbnailFetcher::fetchThumbnail(QString remoteFile)
{
    const QString thumbnailApiPath = QStringLiteral("index.php/apps/files/api/v1/thumbnail");
    const QString thumbnailPath = QStringLiteral("/%1/%2/%3").arg(QString::number(width()),
                                                                  QString::number(height()),
                                                                  remoteFile);
    const QString cacheDirectory = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    const QString cachePath = cacheDirectory + QStringLiteral("/thumbnails") + remoteFile;

    if (QFile(cachePath).exists()) {
        setSource(cachePath);
        return;
    }

    QWebdav* client = getNewWebDav(m_settings, thumbnailApiPath, this);

    if (!m_commandQueue)
        return;

    FileDownloadCommandEntity* thumbnailDownloadCommand =
            new FileDownloadCommandEntity(this->m_commandQueue,
                                          thumbnailPath,
                                          cachePath,
                                          this->m_settings,
                                          client);

    QObject::connect(thumbnailDownloadCommand, &FileDownloadCommandEntity::done, this, [=]() {
        setSource(cachePath);
        Q_EMIT fetchingChanged();
    });
    QObject::connect(this->m_commandQueue, &CommandQueue::runningChanged, this, [=]() {
        Q_EMIT fetchingChanged();
    });

    this->m_commandQueue->enqueue(thumbnailDownloadCommand);
    if (!this->m_commandQueue->isRunning())
        this->m_commandQueue->run();
    Q_EMIT fetchingChanged();
}

bool ThumbnailFetcher::fetching()
{
    if (this->m_commandQueue)
        return this->m_commandQueue->isRunning();
    return false;
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

qint8 ThumbnailFetcher::width()
{
    return this->m_width;
}

void ThumbnailFetcher::setWidth(qint8 v)
{
    if (this->m_width == v)
        return;

    this->m_width = v;
    Q_EMIT widthChanged();
}

qint8 ThumbnailFetcher::height()
{
    return this->m_height;
}

void ThumbnailFetcher::setHeight(qint8 v)
{
    if (this->m_height == v)
        return;

    this->m_height = v;
    Q_EMIT heightChanged();
}
