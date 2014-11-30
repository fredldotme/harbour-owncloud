#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QStandardPaths>

#include <qwebdav.h>
#include "downloadentry.h"
#include "entryinfo.h"

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    DownloadManager(QObject *parent = 0, QWebdav *webdav = 0);

    Q_INVOKABLE DownloadEntry* enqueueDownload(EntryInfo* entry, bool open);
    Q_INVOKABLE bool isNotEnqueued(EntryInfo* entry);

private:
    QWebdav *webdav;
    QQueue<DownloadEntry *> downloadQueue;
    QMutex downloadMutex;

    QString destinationFromMIME(QString mime);

signals:

public slots:
    void handleDownloadCompleted();

};

#endif // DOWNLOADMANAGER_H
