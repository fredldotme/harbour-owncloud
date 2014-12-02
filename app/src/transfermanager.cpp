#include "transfermanager.h"

TransferManager::TransferManager(QObject *parent, OwnCloudBrowser *browser) :
    QObject(parent)
{
    this->browser = browser;
}

void TransferManager::enqueueDownload(EntryInfo* entry, bool open)
{
    downloadMutex.lock();

    QString name = entry->path().mid(entry->path().lastIndexOf("/") + 1);
    QString destination = destinationFromMIME(entry->mimeType()) + "/" + name;
    TransferEntry::TransferDirection direction = TransferEntry::DOWN;

    TransferEntry *newDownload = new TransferEntry(this,
                                                   browser->getWebdav(),
                                                   name,
                                                   entry->path(),
                                                   destination,
                                                   entry->size(),
                                                   direction,
                                                   open);

    connect(newDownload, SIGNAL(downloadCompleted()), this, SLOT(handleDownloadCompleted()), Qt::DirectConnection);
    if(downloadQueue.size() == 0) {
        newDownload->startTransfer();
    }
    downloadQueue.enqueue(newDownload);

    downloadMutex.unlock();
}

void TransferManager::enqueueUpload(QString localPath, QString remotePath)
{

}

void TransferManager::handleDownloadCompleted()
{
    downloadMutex.lock();

    if(!downloadQueue.isEmpty()) {
        disconnect(downloadQueue.head(), SIGNAL(downloadCompleted()), this, SLOT(handleDownloadCompleted()));
        TransferEntry *entry = downloadQueue.dequeue();
        delete entry;
    }

    if(!downloadQueue.isEmpty() && downloadQueue.head())
        downloadQueue.head()->startTransfer();

    downloadMutex.unlock();
}

bool TransferManager::isNotEnqueued(EntryInfo *entry)
{
    downloadMutex.lock();
    for(int i = 0; i < downloadQueue.size(); i++) {
        if(downloadQueue.at(i)->getRemotePath() == entry->path()) {
            return false;
        }
    }
    downloadMutex.unlock();

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
