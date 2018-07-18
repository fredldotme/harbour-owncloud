#include "transfermanager.h"

#include <QStandardPaths>
#include <QMutexLocker>

#include <commands/filedownloadcommandentity.h>
#include <commands/fileuploadcommandentity.h>
#include <commands/remotedirectorycommandentity.h>

TransferManager::TransferManager(QObject *parent, OwnCloudBrowser *browser) :
    QObject(parent)
{
    this->browser = browser;
}

QVariantList TransferManager::getTransfers()
{
    QMutexLocker dLocker(&this->downloadMutex);
    QMutexLocker uLocker(&this->uploadMutex);

    // Show uploads before downloads in the list
    QVariantList transfers;

    for (const CommandEntityInfo& info : m_uploadQueue.queueInformation()) {
        QVariant tmpVariant;
        tmpVariant.setValue(info);
        transfers.append(tmpVariant);
    }

    for (const CommandEntityInfo& info : m_downloadQueue.queueInformation()) {
        QVariant tmpVariant;
        tmpVariant.setValue(info);
        transfers.append(tmpVariant);
    }

    return transfers;
}

bool TransferManager::isTransfering()
{
    return !m_downloadQueue.isEmpty() || !m_uploadQueue.isEmpty();
}

CommandEntity* TransferManager::enqueueDownload(QString remotePath, QString mimeType, bool open)
{
    FileDownloadCommandEntity* downloadCommand = Q_NULLPTR;

    {
        QMutexLocker locker(&this->downloadMutex);

        QString name = remotePath.mid(remotePath.lastIndexOf("/") + 1);
        QString destination = destinationFromMIME(mimeType) + "/" + name;

        downloadCommand = new FileDownloadCommandEntity(&this->m_downloadQueue,
                                                        remotePath,
                                                        destination,
                                                        this->browser->getSettings());

        QObject::connect(downloadCommand, &FileDownloadCommandEntity::done,
                         this, [=]() {
            Q_EMIT downloadComplete(downloadCommand->info());
        });

        if (open) {
            QObject::connect(downloadCommand, &FileDownloadCommandEntity::done, this, [=]() {
                ShellCommand::runCommand(QStringLiteral("xdg-open"), QStringList() << destination);
            });
        }

        this->m_downloadQueue.enqueue(downloadCommand);
        if (!this->m_downloadQueue.isRunning())
            this->m_downloadQueue.run();
    }
    emit transferAdded();
    emit transferingChanged();

    return downloadCommand;
}

void TransferManager::enqueueUpload(QString localPath, QString remotePath)
{
    qDebug() << "enqueued upload";

    FileUploadCommandEntity* uploadCommand = Q_NULLPTR;

    {
        QMutexLocker locker(&this->uploadMutex);

        uploadCommand = new FileUploadCommandEntity(&this->m_downloadQueue,
                                                    localPath,
                                                    remotePath,
                                                    this->browser->getSettings());


        QObject::connect(uploadCommand, &FileDownloadCommandEntity::done,
                         this, [=]() {
            Q_EMIT uploadComplete(uploadCommand->info());
        });

        this->m_uploadQueue.enqueue(uploadCommand);
        if (!this->m_uploadQueue.isRunning())
            this->m_uploadQueue.run();
    }
    emit transferAdded();
    emit transferingChanged();

    return;
}

void TransferManager::refreshDirectoryContents()
{
    qDebug() << Q_FUNC_INFO << "gonna refresh due to succeeded";
    // Refresh to see the current, new mtime
    this->browser->getDirectoryContent(this->browser->getCurrentPath());
}

bool TransferManager::isNotEnqueued(QString remotePath)
{
    {
        QMutexLocker l(&this->downloadMutex);
        for (const CommandEntityInfo& info : this->m_downloadQueue.queueInformation()) {
            if (info.properties().value("remotePath") == remotePath) {
                return false;
            }
        }
    }

    {
        QMutexLocker l(&this->uploadMutex);
        for (const CommandEntityInfo& info : this->m_uploadQueue.queueInformation()) {
            if (info.properties().value("remotePath") == remotePath) {
                return false;
            }
        }
    }

    return true;
}

QString TransferManager::destinationFromMIME(QString mime)
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

