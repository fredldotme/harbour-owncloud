#include "webdavcommandqueue.h"

#include <QDebug>

#ifndef GHOSTCLOUD_UBUNTU_TOUCH
#include <commands/webdav/filedownloadcommandentity.h>
#else
#include <commands/ubuntutouch/utfiledownloadcommandentity.h>
#endif
#include <commands/webdav/fileuploadcommandentity.h>
#include <commands/webdav/mkdavdircommandentity.h>
#include <commands/webdav/davrmcommandentity.h>
#include <commands/webdav/davcopycommandentity.h>
#include <commands/webdav/davmovecommandentity.h>
#include <commands/webdav/davlistcommandentity.h>
#include <commands/webdav/davproppatchcommandentity.h>
#include <commandunit.h>
#include <stdfunctioncommandentity.h>

#include <util/filepathutil.h>
#include <util/shellcommand.h>
#include <util/webdav_utils.h>

#include <nextcloudendpointconsts.h>

#include <sys/types.h>
#include <utime.h>

#include <QDesktopServices>
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#endif

WebDavCommandQueue::WebDavCommandQueue(QObject* parent, AccountBase* settings) :
    CloudStorageProvider(parent, settings)
{
    updateConnectionSettings();
    QObject::connect(this, &WebDavCommandQueue::settingsChanged,
                     this, &WebDavCommandQueue::updateConnectionSettings);
}

void WebDavCommandQueue::updateConnectionSettings()
{
    if (!this->settings())
        return;

    QObject::disconnect(this->settings(), nullptr, nullptr, nullptr);

    // Apply new settings to existing QWebdav object
    if (!this->m_client) {
        this->m_client = getNewWebDav(this->settings(), this);
    } else {
        applySettingsToWebdav(this->settings(), this->m_client);
    }

    // Connect to changes of credentials, certificate, hostname and provider settings
    QObject::connect(this->settings(), &AccountBase::hoststringChanged,
                     this, &WebDavCommandQueue::updateConnectionSettings);
    QObject::connect(this->settings(), &AccountBase::usernameChanged,
                     this, &WebDavCommandQueue::updateConnectionSettings);
    QObject::connect(this->settings(), &AccountBase::passwordChanged,
                     this, &WebDavCommandQueue::updateConnectionSettings);
    QObject::connect(this->settings(), &AccountBase::providerTypeChanged,
                     this, &WebDavCommandQueue::updateConnectionSettings);
    QObject::connect(this->settings(), &AccountBase::customCertChanged,
                     this, &WebDavCommandQueue::updateConnectionSettings);
}

CommandEntity* WebDavCommandQueue::makeDirectoryRequest(const QString dirName,
                                                        const bool enqueue)
{
    MkDavDirCommandEntity* command =
            new MkDavDirCommandEntity(this, dirName, this->getWebdav());

    if (enqueue)
        this->enqueue(command);
    return command;
}

CommandEntity* WebDavCommandQueue::removeRequest(const QString name,
                                                 const bool enqueue)
{
    DavRmCommandEntity* command =
            new DavRmCommandEntity(this, name, this->getWebdav());

    if (enqueue)
        this->enqueue(command);
    return command;
}

CommandEntity* WebDavCommandQueue::moveRequest(const QString from,
                                               const QString to,
                                               const bool enqueue)
{
    DavMoveCommandEntity* command =
            new DavMoveCommandEntity(this, from, to, this->getWebdav());

    if (enqueue)
        this->enqueue(command);
    return command;
}

CommandEntity* WebDavCommandQueue::copyRequest(const QString from,
                                               const QString to,
                                               const bool enqueue)
{
    DavCopyCommandEntity* command =
            new DavCopyCommandEntity(this, from, to, this->getWebdav());

    if (enqueue)
        this->enqueue(command);
    return command;
}

CommandEntity* WebDavCommandQueue::directoryListingRequest(const QString path,
                                                           const bool refresh,
                                                           const bool enqueue)
{
    qDebug() << Q_FUNC_INFO;
    DavListCommandEntity* command =
            new DavListCommandEntity(this, path, refresh, this->getWebdav());

    if (enqueue)
        this->enqueue(command);
    return command;
}

CommandEntity* WebDavCommandQueue::fileDownloadRequest(const QString remotePath,
                                                       const QString mimeType,
                                                       const bool open,
                                                       const QDateTime lastModified,
                                                       const bool enqueue)
{
#ifdef Q_OS_ANDROID
    const QStringList requiredPermissions =
            QStringList("android.permission.WRITE_EXTERNAL_STORAGE");

    QtAndroid::PermissionResultMap permissionResult =
            QtAndroid::requestPermissionsSync(requiredPermissions);

    for (QtAndroid::PermissionResult result : permissionResult) {
        if (result == QtAndroid::PermissionResult::Denied) {
            return Q_NULLPTR;
        }
    }
#endif

    CommandEntity* downloadCommand = Q_NULLPTR;
    CommandEntity* lastModifiedCommand = Q_NULLPTR;
    Q_UNUSED(mimeType);
    QString destination = FilePathUtil::destination(this->settings()) + remotePath;

#ifndef GHOSTCLOUD_UBUNTU_TOUCH
    downloadCommand = new FileDownloadCommandEntity(this, remotePath,
                                                    destination, this->getWebdav());
#else
    downloadCommand = new UtFileDownloadCommandEntity(this, remotePath,
                                                      destination, this->settings());
#endif
    // if lastModified has been provided update the local lastModified information after download
    qDebug() << lastModified;
    if (lastModified.isValid()) {
        lastModifiedCommand = localLastModifiedRequest(destination,
                                                       lastModified);
    }

    const QString fileName = QFileInfo(destination).fileName();
    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("fileDownload");
    info["localPath"] = destination;
    info["remotePath"] = remotePath;
    info["fileName"] = fileName;
    info["remoteFile"] = remotePath + fileName;
    info["fileOpen"] = QVariant::fromValue<bool>(open);
    CommandEntityInfo unitInfo(info);

    CommandUnit* commandUnit = new CommandUnit(this,
    {downloadCommand, lastModifiedCommand}, unitInfo);
    if (enqueue) {
        this->enqueue(commandUnit);
    }
    return commandUnit;
}

CommandEntity* WebDavCommandQueue::fileUploadRequest(const QString localPath,
                                                     const QString remotePath,
                                                     const QDateTime lastModified,
                                                     const bool enqueue)
{
#ifdef Q_OS_ANDROID
    const QStringList requiredPermissions =
            QStringList("android.permission.READ_EXTERNAL_STORAGE");

    QtAndroid::PermissionResultMap permissionResult =
            QtAndroid::requestPermissionsSync(requiredPermissions);

    for (QtAndroid::PermissionResult result : permissionResult) {
        if (result == QtAndroid::PermissionResult::Denied) {
            return Q_NULLPTR;
        }
    }
#endif

    QString newLocalPath = localPath;
    if (newLocalPath.startsWith("file://")) {
        newLocalPath = newLocalPath.mid(7);
    }
    qDebug() << newLocalPath;
    const QString fileName = newLocalPath.mid(newLocalPath.lastIndexOf('/') + 1);

    qDebug() << "upload requested";
    FileUploadCommandEntity* uploadCommand =
            new FileUploadCommandEntity(this, newLocalPath, remotePath, this->getWebdav());
    CommandEntity* lastModifiedCommand = Q_NULLPTR;

    // if lastModified has been provided update the remote lastModified information afterwards
    qDebug() << lastModified;
    if (lastModified.isValid()) {
        lastModifiedCommand = remoteLastModifiedRequest(remotePath + fileName, lastModified);
    }

    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("fileUpload");
    info["localPath"] = newLocalPath;
    info["remotePath"] = remotePath;
    info["fileName"] = fileName;
    info["remoteFile"] = remotePath + fileName;

    CommandUnit* commandUnit = new CommandUnit(this,
    {uploadCommand, lastModifiedCommand}, CommandEntityInfo(info));

    if (enqueue)
        this->enqueue(commandUnit);
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

        newLocalLastModified.actime = time(nullptr);
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

CommandEntity* WebDavCommandQueue::remoteLastModifiedRequest(const QString &destination,
                                                             const QDateTime &lastModified)
{
    QWebdav::PropValues props;
    QMap<QString, QVariant> propMap;

    // Last modified in seconds
    propMap["lastmodified"] = static_cast<QVariant>(lastModified.toMSecsSinceEpoch() / 1000);
    props["DAV:"] = propMap;

    DavPropPatchCommandEntity* propPatchCommand =
            new DavPropPatchCommandEntity(this, destination,
                                          props, this->getWebdav());
    return propPatchCommand;
}
