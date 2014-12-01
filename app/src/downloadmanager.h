#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QStandardPaths>

#include "owncloudbrowser.h"
#include "downloadentry.h"
#include "entryinfo.h"

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    DownloadManager(QObject *parent = 0, OwnCloudBrowser *browser = 0);

    Q_INVOKABLE DownloadEntry* enqueueDownload(EntryInfo* entry, bool open);
    Q_INVOKABLE bool isNotEnqueued(EntryInfo* entry);

private:
    OwnCloudBrowser *browser;
    QQueue<DownloadEntry *> downloadQueue;
    QMutex downloadMutex;

    QString destinationFromMIME(QString mime);

signals:

public slots:
    void handleDownloadCompleted();

};

#endif // DOWNLOADMANAGER_H
