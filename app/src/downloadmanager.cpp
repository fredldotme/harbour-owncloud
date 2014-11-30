#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent, QWebdav *webdav) :
    QObject(parent)
{
    this->webdav = webdav;
}

void DownloadManager::enqueueDownload(EntryInfo* entry)
{
    downloadMutex.lock();

    qDebug() << "Info: " << entry->size();
    QString name = entry->path().mid(entry->path().lastIndexOf("/") + 1);
    QString destination = destinationFromMIME(entry->mimeType()) + "/" + name;

    DownloadEntry *newDownload = new DownloadEntry(this, webdav, name, entry->path(), destination, entry->size());
    connect(newDownload, SIGNAL(downloadCompleted()), this, SLOT(handleDownloadCompleted()));
    if(downloadQueue.size() == 0) {
        newDownload->startDownload();
    }
    downloadQueue.enqueue(newDownload);

    downloadMutex.unlock();
}

void DownloadManager::handleDownloadCompleted()
{
    downloadMutex.lock();
    downloadQueue.dequeue();

    if(downloadQueue.size() > 0)
        downloadQueue.head()->startDownload();

    downloadMutex.unlock();
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
