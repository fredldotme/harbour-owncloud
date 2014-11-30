#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QQueue>
#include <QStandardPaths>

#include <qwebdav.h>
#include "downloadentry.h"

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    DownloadManager(QObject *parent = 0, QWebdav *webdav = 0);

    Q_INVOKABLE void enqueueDownload(QString remotePath, QString mimeType, qint64 size);

private:
    QWebdav *webdav;
    QQueue<DownloadEntry *> downloadQueue;

    QString destinationFromMIME(QString mime);

signals:

public slots:
    void handleDownloadCompleted();

};

#endif // DOWNLOADMANAGER_H
