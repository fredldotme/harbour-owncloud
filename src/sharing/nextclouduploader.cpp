#include "nextclouduploader.h"

#include <QDebug>
#include <settings/inifilesettings.h>

NextcloudUploader::NextcloudUploader(QObject *parent) : MediaTransferInterface(parent)
{
    IniFileSettings::instance()->readSettings();
    this->m_commandQueue.setSettings(IniFileSettings::instance());

    QObject::connect(&this->m_commandQueue, &WebDavCommandQueue::commandFinished,
                     this, [=](CommandReceipt receipt) {
        if (receipt.finished)
            setStatus(MediaTransferInterface::TransferFinished);
        else
            setStatus(MediaTransferInterface::TransferCanceled);
    });
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

    CommandEntity* transferCommand =
            this->m_commandQueue.fileUploadRequest(localPath, remotePath);
    if (!transferCommand) {
        setStatus(MediaTransferInterface::TransferInterrupted);
        return;
    }

    QObject::connect(transferCommand, &CommandEntity::progressChanged,
                     this, [=]() {
        if (!transferCommand)
            return;
        setProgress(transferCommand->progress());
    });

    this->m_commandQueue.run();
    setStatus(MediaTransferInterface::TransferStarted);
}

void NextcloudUploader::cancel()
{
    if (!mediaItem())
        return;

    const QMap<QString, QVariant> userData = mediaItem()->value(MediaItem::UserData).toMap();
    const QString remotePath = userData.value("remoteDir").toString();

    for (QVariant entry : this->m_commandQueue.queue()) {
        CommandEntity* transferCommand = entry.value<CommandEntity*>();
        if (!transferCommand)
            continue;

        if (transferCommand->info().property(QStringLiteral("remotePath")) == remotePath) {
            qDebug() << "aborting upload to" << remotePath;
            setStatus(MediaTransferInterface::TransferCanceled);
            transferCommand->abort();
            break;
        }
    }
}
