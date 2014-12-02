#ifndef TRANSFERMANAGER_H
#define TRANSFERMANAGER_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QStandardPaths>

#include "owncloudbrowser.h"
#include "transferentry.h"
#include "entryinfo.h"

class TransferManager : public QObject
{
    Q_OBJECT
public:
    TransferManager(QObject *parent = 0, OwnCloudBrowser *browser = 0);

    Q_INVOKABLE void enqueueDownload(EntryInfo* entry, bool open);
    Q_INVOKABLE void enqueueUpload(QString localPath, QString remotePath);
    Q_INVOKABLE bool isNotEnqueued(EntryInfo* entry);

private:
    OwnCloudBrowser *browser;
    QQueue<TransferEntry *> downloadQueue;
    QQueue<TransferEntry *> uploadQueue;
    QMutex downloadMutex;
    QMutex uploadMutex;

    QString destinationFromMIME(QString mime);

signals:

public slots:
    void handleDownloadCompleted();

};

#endif // TRANSFERMANAGER_H
