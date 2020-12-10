#include "ghostclouduploader.h"

#include <QDebug>
#include <settings/nextcloudsettingsbase.h>

GhostCloudUploader::GhostCloudUploader(QObject *parent) : MediaTransferInterface(parent)
{
    QObject::connect(&this->m_commandQueue, &WebDavCommandQueue::commandFinished,
                     this, [=](CommandReceipt receipt) {
        if (receipt.finished)
            setStatus(MediaTransferInterface::TransferFinished);
        else
            setStatus(MediaTransferInterface::TransferCanceled);
    });
}

GhostCloudUploader::~GhostCloudUploader()
{
}

QString GhostCloudUploader::displayName() const
{
    return tr("GhostCloud");
}

QUrl GhostCloudUploader::serviceIcon() const
{
    return QUrl("image://theme/icon-s-cloud-upload");
}

bool GhostCloudUploader::cancelEnabled() const
{
    return true;
}

bool GhostCloudUploader::restartEnabled() const
{
    return false;
}

void GhostCloudUploader::start()
{
    if (!mediaItem())
        return;

    const QMap<QString, QVariant> userData = mediaItem()->value(MediaItem::UserData).toMap();
    QString localPath = userData.value("localPath").toString();
    if (localPath.startsWith("file://"))
        localPath = localPath.mid(7);
    const QString remotePath = userData.value("remoteDir").toString();
    AccountBase* account = new AccountBase();
    const QString hoststring = userData.value("hoststring").toString();
    const QString username = userData.value("username").toString();
    const QString password = userData.value("password").toString();

    account->setHoststring(hoststring);
    account->setUsername(username);
    account->setPassword(password);

    this->m_commandQueue.setSettings(account);

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

void GhostCloudUploader::cancel()
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
