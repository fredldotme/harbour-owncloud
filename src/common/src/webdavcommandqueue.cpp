#include "webdavcommandqueue.h"

#include <commands/webdav/filedownloadcommandentity.h>
#include <commands/webdav/fileuploadcommandentity.h>
#include <commands/webdav/mkdavdircommandentity.h>
#include <commands/webdav/davrmcommandentity.h>
#include <commands/webdav/davcopycommandentity.h>
#include <commands/webdav/davmovecommandentity.h>
#include <commands/webdav/davlistcommandentity.h>
#include <commands/stdfunctioncommandentity.h>

#include <util/filepathutil.h>
#include <util/shellcommand.h>

#include <net/webdav_utils.h>

#include <nextcloudendpointconsts.h>

WebDavCommandQueue::WebDavCommandQueue(QObject* parent, NextcloudSettingsBase* settings) :
    CommandQueue(parent)
{
    this->m_settings = settings;

    QObject::connect(this, &WebDavCommandQueue::settingsChanged,
                     this, [=]() {
        if (this->m_client) {
            this->m_client->deleteLater();
            this->m_client = Q_NULLPTR;
        }

        if (!this->m_settings)
            return;

        this->m_settings->readSettings();
        this->m_client = getNewWebDav(this->m_settings, NEXTCLOUD_ENDPOINT_WEBDAV, this);
    });
}

NextcloudSettingsBase* WebDavCommandQueue::settings()
{
    return this->m_settings;
}

void WebDavCommandQueue::setSettings(NextcloudSettingsBase *v)
{
    if (this->m_settings == v)
        return;

    this->m_settings = v;
    Q_EMIT settingsChanged();
}

CommandEntity* WebDavCommandQueue::makeDirectoryRequest(QString dirName)
{
    MkDavDirCommandEntity* command =
            new MkDavDirCommandEntity(this, dirName, this->getWebdav());

    enqueue(command);
    return command;
}

CommandEntity* WebDavCommandQueue::removeRequest(QString name)
{
    DavRmCommandEntity* command =
            new DavRmCommandEntity(this, name, this->getWebdav());

    enqueue(command);
    return command;
}

CommandEntity* WebDavCommandQueue::moveRequest(QString from, QString to)
{
    DavMoveCommandEntity* command =
            new DavMoveCommandEntity(this, from, to, this->getWebdav());

    enqueue(command);
    return command;
}

CommandEntity* WebDavCommandQueue::copyRequest(QString from, QString to)
{
    DavCopyCommandEntity* command =
            new DavCopyCommandEntity(this, from, to, this->getWebdav());

    enqueue(command);
    return command;
}

CommandEntity* WebDavCommandQueue::directoryListingRequest(QString path)
{
    qDebug() << Q_FUNC_INFO;
    DavListCommandEntity* command =
            new DavListCommandEntity(this, path, this->getWebdav());

    enqueue(command);
    return command;
}

CommandEntity* WebDavCommandQueue::fileDownloadRequest(QString remotePath, QString mimeType, bool open)
{
    FileDownloadCommandEntity* downloadCommand = Q_NULLPTR;

    QString name = remotePath.mid(remotePath.lastIndexOf("/") + 1);
    QString destination = FilePathUtil::destinationFromMIME(mimeType) + "/" + name;

    downloadCommand = new FileDownloadCommandEntity(this, remotePath, destination, this->getWebdav());
    enqueue(downloadCommand);

    if (open) {
        StdFunctionCommandEntity* executeCommand = new StdFunctionCommandEntity(this, [destination]() {
            qDebug() << destination;
            if (!QFile(destination).exists())
                return;
            ShellCommand::runCommand(QStringLiteral("xdg-open"), QStringList() << destination);
        });
        enqueue(executeCommand);
    }

    return downloadCommand;
}

CommandEntity* WebDavCommandQueue::fileUploadRequest(QString localPath, QString remotePath)
{
    qDebug() << "upload requested";
    FileUploadCommandEntity* uploadCommand =
            new FileUploadCommandEntity(this, localPath, remotePath, this->getWebdav());

    enqueue(uploadCommand);
    return uploadCommand;
}

