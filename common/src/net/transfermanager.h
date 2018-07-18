#ifndef TRANSFERMANAGER_H
#define TRANSFERMANAGER_H

#include <QObject>
#include <QQueue>
#include <QMutex>

#include <net/transferentry.h>
#include <net/owncloudbrowser.h>

#include <commandqueue.h>

class TransferManager : public QObject
{
    Q_OBJECT

public:
    TransferManager(QObject *parent = 0, OwnCloudBrowser *browser = 0);

    Q_PROPERTY(bool transfering READ isTransfering NOTIFY transferingChanged)
    Q_PROPERTY(CommandQueue* downloadQueue READ downloadQueue CONSTANT)
    Q_PROPERTY(CommandQueue* uploadQueue READ uploadQueue CONSTANT)
    Q_PROPERTY(CommandQueue* miscQueue READ miscQueue CONSTANT)

    Q_INVOKABLE CommandEntity* enqueueDownload(QString remotePath, QString mimeType, bool open);
    Q_INVOKABLE void enqueueUpload(QString localPath, QString remotePath);
    Q_INVOKABLE bool isNotEnqueued(QString remotePath);
    Q_INVOKABLE QVariantList getTransfers();

private:
    CommandQueue* downloadQueue() { return &this->m_downloadQueue; }
    CommandQueue* uploadQueue() { return &this->m_uploadQueue; }
    CommandQueue* miscQueue() { return &this->m_miscQueue; }

    OwnCloudBrowser *browser = Q_NULLPTR;
    CommandQueue m_downloadQueue;
    CommandQueue m_uploadQueue;
    CommandQueue m_miscQueue;
    QMutex downloadMutex;
    QMutex uploadMutex;

    QString destinationFromMIME(QString mime);

    bool isTransfering();

signals:
    void transferAdded();
    void transferingChanged();
    void downloadComplete(CommandEntityInfo info);
    void downloadFailed(CommandEntityInfo info);
    void uploadComplete(CommandEntityInfo info);
    void uploadFailed(CommandEntityInfo info);

public slots:
    void refreshDirectoryContents();
};

#endif // TRANSFERMANAGER_H
