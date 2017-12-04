#include "transfermanager.h"
#include <QStandardPaths>

TransferManager::TransferManager(QObject *parent, OwnCloudBrowser *browser) :
    QObject(parent)
{
    this->browser = browser;
}

QVariantList TransferManager::getTransfers()
{
    downloadMutex.lock();
    uploadMutex.lock();

    // Show uploads before downloads in the list
    QVariantList transfers;

    foreach(TransferEntry *entry, uploadQueue) {
        QVariant tmpVariant;
        tmpVariant.setValue(entry);
        transfers.append(tmpVariant);
    }

    foreach(TransferEntry *entry, downloadQueue) {
        QVariant tmpVariant;
        tmpVariant.setValue(entry);
        transfers.append(tmpVariant);
    }

    downloadMutex.unlock();
    uploadMutex.unlock();

    return transfers;
}

bool TransferManager::isTransfering()
{
    return !downloadQueue.isEmpty() || !uploadQueue.isEmpty();
}

TransferEntry* TransferManager::enqueueDownload(EntryInfo* entry, bool open)
{
    downloadMutex.lock();

    QString name = entry->path().mid(entry->path().lastIndexOf("/") + 1);
    QString destination = destinationFromMIME(entry->mimeType()) + "/" + name;
    TransferEntry::TransferDirection direction = TransferEntry::DOWN;

    TransferEntry *newDownload = new TransferEntry(this,
                                                   browser->getNewWebdav(),
                                                   name,
                                                   entry->path(),
                                                   destination,
                                                   entry->size(),
                                                   direction,
                                                   open,
                                                   QStringList());

    newDownload->setLastModified(entry->modTime());

    connect(newDownload, &TransferEntry::transferCompleted, this, &TransferManager::handleDownloadCompleted, Qt::DirectConnection);
    if(downloadQueue.isEmpty()) {
        newDownload->startTransfer();
    }
    downloadQueue.enqueue(newDownload);

    downloadMutex.unlock();
    emit transferAdded();
    emit transferingChanged();

    return newDownload;
}

void TransferManager::enqueueUpload(QString localPath, QString remotePath)
{
    qDebug() << "enqeued upload";

    uploadMutex.lock();
    QFile localFile(localPath);
    qint64 size = localFile.size();
    bool open = false;

    QString name = localPath.mid(localPath.lastIndexOf("/") + 1);
    TransferEntry::TransferDirection direction = TransferEntry::UP;

    TransferEntry *newUpload = new TransferEntry(this,
                                                   browser->getNewWebdav(),
                                                   name,
                                                   remotePath,
                                                   localPath,
                                                   size,
                                                   direction,
                                                   open,
                                                   QStringList());

    connect(newUpload, &TransferEntry::transferCompleted, this, &TransferManager::handleUploadCompleted, Qt::DirectConnection);
    if(uploadQueue.isEmpty()) {
        newUpload->startTransfer();
    }
    uploadQueue.enqueue(newUpload);
    uploadMutex.unlock();
    emit transferAdded();
    emit transferingChanged();
}

void TransferManager::handleDownloadCompleted()
{
    TransferEntry* entry = Q_NULLPTR;;
    bool success = false;
    downloadMutex.lock();

    if(!downloadQueue.isEmpty()) {
        disconnect(downloadQueue.head(), SIGNAL(transferCompleted(bool)), this, SLOT(handleDownloadCompleted()));
        entry = downloadQueue.dequeue();
        success = entry->succeeded();
        connect(entry, &TransferEntry::localMtimeFailed, this, &TransferManager::localMtimeFailed);
        entry->deleteLater();
    }

    if(!downloadQueue.isEmpty() && downloadQueue.head())
        downloadQueue.head()->startTransfer();

    downloadMutex.unlock();

    if(success)
        emit downloadComplete(entry);
    else
        emit downloadFailed(entry);
    emit transferingChanged();
}

void TransferManager::handleUploadCompleted()
{
    TransferEntry* entry = Q_NULLPTR;;
    bool success;
    uploadMutex.lock();

    if(!uploadQueue.isEmpty()) {
        disconnect(uploadQueue.head(), SIGNAL(transferCompleted(bool)), this, SLOT(handleUploadCompleted()));
        entry = uploadQueue.dequeue();
        success = entry->succeeded();
        connect(entry, &TransferEntry::remoteMtimeSucceeded, this, &TransferManager::refreshDirectoryContents);
        connect(entry, &TransferEntry::remoteMtimeFailed, this, &TransferManager::remoteMtimeFailed);
        entry->deleteLater();
    }

    if(!uploadQueue.isEmpty() && uploadQueue.head())
        uploadQueue.head()->startTransfer();

    uploadMutex.unlock();

    if (entry) {
        if(success)
            emit uploadComplete(entry, entry->getRemotePath());
        else
            emit uploadFailed(entry);
    }

    emit transferingChanged();
}

void TransferManager::refreshDirectoryContents()
{
    qDebug() << Q_FUNC_INFO << "gonna refresh due to succeeded";
    // Refresh to see the current, new mtime
    this->browser->getDirectoryContent(this->browser->getCurrentPath());
}

bool TransferManager::isNotEnqueued(EntryInfo *entry)
{
    if(entry == NULL)
        return false;

    downloadMutex.lock();
    for(int i = 0; i < downloadQueue.size(); i++) {
        if(downloadQueue.at(i)->getRemotePath() == entry->path()) {
            return false;
        }
    }
    downloadMutex.unlock();

    uploadMutex.lock();
    for(int i = 0; i < uploadQueue.size(); i++) {
        if(uploadQueue.at(i)->getRemotePath() == entry->path()) {
            return false;
        }
    }
    uploadMutex.unlock();

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
    } else {
        location = QStandardPaths::DownloadLocation;
    }

    return QStandardPaths::writableLocation(location);
}

