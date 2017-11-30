#include "nextclouduploader.h"

#include <QDebug>
#include <globaltransfermanager.h>
#include <globalowncloudbrowser.h>

#define GLOBAL_TRANSFER_MANAGER GlobalTransferManager::instance(GlobalOwncloudBrowser::instance())

NextcloudUploader::NextcloudUploader(QObject *parent) : MediaTransferInterface(parent)
{
    // Initialize singletons
    GLOBAL_TRANSFER_MANAGER;
}

NextcloudUploader::~NextcloudUploader()
{
}

QString NextcloudUploader::displayName() const
{
    return tr("Nextcloud");
}

QUrl NextcloudUploader::serviceIcon() const
{
    return QUrl("image://theme/icon-s-cloud-upload");
}

bool NextcloudUploader::cancelEnabled() const
{
    return true;
}

bool NextcloudUploader::restartEnabled() const
{
    return false;
}

void NextcloudUploader::start()
{
    if (!mediaItem())
        return;

    const QMap<QString, QVariant> userData = mediaItem()->value(MediaItem::UserData).toMap();
    QString localPath = userData.value("localPath").toString();
    if (localPath.startsWith("file://"))
        localPath = localPath.mid(7);
    const QString remotePath = userData.value("remoteDir").toString();

    GLOBAL_TRANSFER_MANAGER->enqueueUpload(localPath, remotePath);
    setStatus(MediaTransferInterface::TransferStarted);

    for (QVariant entry : GLOBAL_TRANSFER_MANAGER->getTransfers()) {
        TransferEntry* transferEntry = entry.value<TransferEntry*>();
        if (!transferEntry)
            continue;

        if (transferEntry->getRemotePath() == remotePath) {
            connect(transferEntry, &TransferEntry::progressChanged, this, [=](qreal progress, QString remotePath) {
                setProgress(progress);
            });
            connect(transferEntry, &TransferEntry::transferCompleted, this, [=](bool success) {
                if (success)
                    setStatus(MediaTransferInterface::TransferFinished);
                else
                    setStatus(MediaTransferInterface::TransferInterrupted);
            });

            break;
        }
    }
}

void NextcloudUploader::cancel()
{
    if (!mediaItem())
        return;

    const QMap<QString, QVariant> userData = mediaItem()->value(MediaItem::UserData).toMap();
    const QString remotePath = userData.value("remoteDir").toString();

    for (QVariant entry : GLOBAL_TRANSFER_MANAGER->getTransfers()) {
        TransferEntry* transferEntry = entry.value<TransferEntry*>();
        if (!transferEntry)
            continue;

        if (transferEntry->getRemotePath() == remotePath) {
            setStatus(MediaTransferInterface::TransferCanceled);
            transferEntry->cancelTransfer();
            break;
        }
    }
}
