#include "webdavcommandqueue.h"

#include <QDebug>

#include <commands/webdav/filedownloadcommandentity.h>
#include <commands/webdav/fileuploadcommandentity.h>
#include <commands/webdav/mkdavdircommandentity.h>
#include <commands/webdav/davrmcommandentity.h>
#include <commands/webdav/davcopycommandentity.h>
#include <commands/webdav/davmovecommandentity.h>
#include <commands/webdav/davlistcommandentity.h>
#include <commands/webdav/davproppatchcommandentity.h>
#include <commands/stdfunctioncommandentity.h>
#include <commandunit.h>

#include <util/filepathutil.h>
#include <util/shellcommand.h>
#include <util/webdav_utils.h>

#include <nextcloudendpointconsts.h>

#include <sys/types.h>
#include <utime.h>

WebDavCommandQueue::WebDavCommandQueue(QObject* parent, NextcloudSettingsBase* settings) :
    CommandQueue(parent)
{
    this->m_settings = settings;
    this->m_client = getNewWebDav(this->m_settings, this);

    QObject::connect(this, &WebDavCommandQueue::settingsChanged,
                     this, &WebDavCommandQueue::updateConnectionSettings);
}

NextcloudSettingsBase* WebDavCommandQueue::settings()
{
    return this->m_settings;
}

void WebDavCommandQueue::setSettings(NextcloudSettingsBase *v)
{
    if (this->m_settings == v)
        return;

    // Disconnect from current settings object if it exists
    if (this->m_settings)
        QObject::disconnect(this->m_settings, 0, 0, 0);

    this->m_settings = v;

    // Connect to changes of credentials, certificate and hostname settings
    if (this->m_settings) {
        QObject::connect(this->m_settings, &NextcloudSettingsBase::hoststringChanged,
                         this, &WebDavCommandQueue::updateConnectionSettings);
        QObject::connect(this->m_settings, &NextcloudSettingsBase::usernameChanged,
                         this, &WebDavCommandQueue::updateConnectionSettings);
        QObject::connect(this->m_settings, &NextcloudSettingsBase::passwordChanged,
                         this, &WebDavCommandQueue::updateConnectionSettings);
        QObject::connect(this->m_settings, &NextcloudSettingsBase::customCertChanged,
                         this, &WebDavCommandQueue::updateConnectionSettings);
    }
    Q_EMIT settingsChanged();
}

void WebDavCommandQueue::updateConnectionSettings()
{
    if (!this->m_settings)
        return;

    // Apply settings to new or existing QWebdav object
    if (!this->m_client) {
        this->m_client = getNewWebDav(this->m_settings, this);
    } else {
        applySettingsToWebdav(this->m_settings, this->m_client);
    }
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

CommandEntity* WebDavCommandQueue::directoryListingRequest(QString path, bool refresh)
{
    qDebug() << Q_FUNC_INFO;
    DavListCommandEntity* command =
            new DavListCommandEntity(this, path, refresh, this->getWebdav());

    enqueue(command);
    return command;
}

CommandEntity* WebDavCommandQueue::fileDownloadRequest(QString remotePath,
                                                       QString mimeType,
                                                       bool open,
                                                       QDateTime lastModified)
{
    FileDownloadCommandEntity* downloadCommand = Q_NULLPTR;
    CommandEntity* lastModifiedCommand = Q_NULLPTR;
    CommandEntity* openFileCommand = Q_NULLPTR;


    QString name = remotePath.mid(remotePath.lastIndexOf("/") + 1);
    QString destination = FilePathUtil::destinationFromMIME(mimeType) + "/" + name;

    downloadCommand = new FileDownloadCommandEntity(this, remotePath,
                                                    destination, this->getWebdav());

    // if lastModified has been provided update the local lastModified information after download
    qDebug() << lastModified;
    if (lastModified.isValid()) {
        lastModifiedCommand = localLastModifiedRequest(destination,
                                                       lastModified);
    }

    if (open) {
        openFileCommand = openFileRequest(destination);
    }

    const QString fileName = QFileInfo(destination).fileName();
    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("fileDownload");
    info["localPath"] = destination;
    info["remotePath"] = remotePath;
    info["fileName"] = fileName;
    info["remoteFile"] = remotePath + fileName;
    CommandEntityInfo unitInfo(info);

    CommandUnit* commandUnit = new CommandUnit(this,
    {downloadCommand, lastModifiedCommand}, unitInfo);

    enqueue(commandUnit);
    enqueue(openFileCommand);
    return commandUnit;
}

CommandEntity* WebDavCommandQueue::fileUploadRequest(QString localPath,
                                                     QString remotePath,
                                                     QDateTime lastModified)
{
    const QString fileName = localPath.mid(localPath.lastIndexOf('/') + 1);

    qDebug() << "upload requested";
    FileUploadCommandEntity* uploadCommand =
            new FileUploadCommandEntity(this, localPath, remotePath, this->getWebdav());
    CommandEntity* lastModifiedCommand = Q_NULLPTR;

    // if lastModified has been provided update the remote lastModified information afterwards
    qDebug() << lastModified;
    if (lastModified.isValid()) {
        lastModifiedCommand = remoteLastModifiedRequest(remotePath + fileName, lastModified);
    }

    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("fileUpload");
    info["localPath"] = localPath;
    info["remotePath"] = remotePath;
    info["fileName"] = fileName;
    info["remoteFile"] = remotePath + fileName;

    CommandUnit* commandUnit = new CommandUnit(this,
    {uploadCommand, lastModifiedCommand}, CommandEntityInfo(info));

    enqueue(commandUnit);
    return uploadCommand;
}

CommandEntity* WebDavCommandQueue::localLastModifiedRequest(const QString &destination,
                                                            const QDateTime &lastModified)
{
    StdFunctionCommandEntity* updateLocalLastModifiedCommand =
            new StdFunctionCommandEntity(this, [destination, lastModified]() {
        if (!QFile(destination).exists())
            return;
        struct utimbuf newLocalLastModified;
        int utimesuccess;

        newLocalLastModified.actime = time(NULL);
        newLocalLastModified.modtime = lastModified.toMSecsSinceEpoch() / 1000; // seconds

        utimesuccess = utime(destination.toStdString().c_str(), &newLocalLastModified);
        if (utimesuccess != 0) {
            qWarning() << "failed to update local last modified time to"
                       << lastModified;
        } else {
            qDebug() << "Local last modified " << newLocalLastModified.modtime;
        }
    });

    return updateLocalLastModifiedCommand;
}

CommandEntity* WebDavCommandQueue::openFileRequest(const QString &destination)
{
    const QString fileName = QFileInfo(destination).fileName();
    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("fileOpen");
    info["localPath"] = destination;
    info["fileName"] = fileName;
    CommandEntityInfo commandInfo(info);

    StdFunctionCommandEntity* executeCommand =
            new StdFunctionCommandEntity(this, [destination]() {
        const bool fileExists = QFile(destination).exists();
        qDebug() << destination << fileExists;
        if (!fileExists)
            return;
        ShellCommand::runCommand(QStringLiteral("xdg-open"), QStringList() << destination);
    }, commandInfo);

    return executeCommand;
}

CommandEntity* WebDavCommandQueue::remoteLastModifiedRequest(const QString &destination,
                                                             const QDateTime &lastModified)
{
    QWebdav::PropValues props;
    QMap<QString, QVariant> propMap;

    // Last modified in seconds
    propMap["lastmodified"] = (QVariant)(lastModified.toMSecsSinceEpoch() / 1000);
    props["DAV:"] = propMap;


    DavPropPatchCommandEntity* propPatchCommand =
            new DavPropPatchCommandEntity(this, destination,
                                          props, this->getWebdav());
    return propPatchCommand;
}
