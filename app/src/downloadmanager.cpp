#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent, QWebdav *webdav) :
    QObject(parent)
{
    this->webdav = webdav;
}

DownloadEntry* DownloadManager::enqueueDownload(EntryInfo* entry)
{
    downloadMutex.lock();

    qDebug() << "Info: " << entry->size();
    QString name = entry->path().mid(entry->path().lastIndexOf("/") + 1);
    QString destination = destinationFromMIME(entry->mimeType()) + "/" + name;

    DownloadEntry *newDownload = new DownloadEntry(this, webdav, name, entry->path(), destination, entry->size());
    connect(newDownload, SIGNAL(downloadCompleted()), this, SLOT(handleDownloadCompleted()), Qt::DirectConnection);
    if(downloadQueue.size() == 0) {
        newDownload->startDownload();
    }
    downloadQueue.enqueue(newDownload);

    downloadMutex.unlock();

    return newDownload;
}

void DownloadManager::handleDownloadCompleted()
{
    downloadMutex.lock();

    if(!downloadQueue.isEmpty())
        downloadQueue.dequeue();

    if(!downloadQueue.isEmpty() && downloadQueue.head())
        downloadQueue.head()->startDownload();

    downloadMutex.unlock();
}

bool DownloadManager::isNotEnqueued(EntryInfo *entry)
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

QString DownloadManager::destinationFromMIME(QString mime)
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
