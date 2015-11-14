#include "transfermanager.h"

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
                                                   open);

    connect(newDownload, SIGNAL(transferCompleted(bool)), this, SLOT(handleDownloadCompleted()), Qt::DirectConnection);
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

    QString name = localPath.mid(localPath.lastIndexOf("/") + 1);
    TransferEntry::TransferDirection direction = TransferEntry::UP;

    TransferEntry *newUpload = new TransferEntry(this,
                                                   browser->getNewWebdav(),
                                                   name,
                                                   remotePath,
                                                   localPath,
                                                   size,
                                                   direction);

    connect(newUpload, SIGNAL(transferCompleted(bool)), this, SLOT(handleUploadCompleted()), Qt::DirectConnection);
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
    TransferEntry* entry;
    bool success;
    downloadMutex.lock();

    if(!downloadQueue.isEmpty()) {
        disconnect(downloadQueue.head(), SIGNAL(transferCompleted(bool)), this, SLOT(handleDownloadCompleted()));
        entry = downloadQueue.dequeue();
        success = entry->getProgress() == 1.0;
        connect(this, SIGNAL(downloadComplete(TransferEntry*)), this, SLOT(setLocalLastModified(TransferEntry*)));
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
    TransferEntry* entry;
    bool success;
    uploadMutex.lock();

    if(!uploadQueue.isEmpty()) {
        disconnect(uploadQueue.head(), SIGNAL(transferCompleted(bool)), this, SLOT(handleUploadCompleted()));
        entry = uploadQueue.dequeue();
        success = entry->getProgress() == 1.0;
        connect(this, SIGNAL(uploadComplete(TransferEntry*, QString)), this, SLOT(setRemoteLastModified(TransferEntry*, QString)));
        entry->deleteLater();
    }

    if(!uploadQueue.isEmpty() && uploadQueue.head())
        uploadQueue.head()->startTransfer();

    uploadMutex.unlock();

    Q_ASSERT(entry); // To ensure entry->deleteLater(); hasn't reaped yet
    if(success)
        emit uploadComplete(entry, entry->getRemotePath());
    else
        emit uploadFailed(entry);

    emit transferingChanged();
}

void TransferManager::setLocalLastModified(TransferEntry* entry)
{
    // TODO: set last modified
    qDebug() << "Last modified?" << entry->getLastModified().toString("yyyy-MM-ddThh:mm:ss.zzz+t");
    disconnect(this, SIGNAL(downloadComplete(TransferEntry*)), this, SLOT(setLocalLastModified(TransferEntry*)));
}

void TransferManager::setRemoteLastModified(TransferEntry *entry, QString remotePath)
{
    Q_ASSERT(remotePath == entry->getRemotePath()); // Technicality for QML

    QWebdav::PropValues props;
    QMap<QString, QVariant> propMap;
    QString remoteName = remotePath + entry->getName();
    qint64 lastModified = entry->getLastModified().toMSecsSinceEpoch() / 1000; // seconds
    QWebdav* webdav = this->browser->getNewWebdav(); // entry's webdav is private

    webdav->setParent(this);
    connect(webdav, &QWebdav::finished, this, &TransferManager::setRemoteMtimeFinished);

    propMap["lastmodified"] = (QVariant) lastModified;
    props["DAV:"] = propMap;

    webdav->proppatch(remoteName, props);
    qDebug() << "Remote last modified " << lastModified;
    disconnect(this, SIGNAL(uploadComplete(TransferEntry*, QString)), this, SLOT(setRemoteLastModified(TransferEntry*, QString)));
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


void TransferManager::setRemoteMtimeFinished(QNetworkReply *networkReply)
{
    QVariant attr = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = attr.toInt();
    qDebug() << "setting mtime status " << status;
    if (status < 200 || status >= 300) {
        emit remoteMtimeFailed(status);
    }
}
