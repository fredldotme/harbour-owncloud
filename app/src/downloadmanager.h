#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QQueue>
#include <QStandardPaths>

#include <qwebdav.h>
#include "downloadentry.h"
#include "entryinfo.h"

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    DownloadManager(QObject *parent = 0, QWebdav *webdav = 0);

    Q_INVOKABLE void enqueueDownload(EntryInfo* entry);

private:
    QWebdav *webdav;
    QQueue<DownloadEntry *> downloadQueue;

    QString destinationFromMIME(QString mime);

signals:

public slots:
    void handleDownloadCompleted();

};

#endif // DOWNLOADMANAGER_H
