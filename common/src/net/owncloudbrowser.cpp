#include "owncloudbrowser.h"

#include <commands/filedownloadcommandentity.h>
#include <commands/fileuploadcommandentity.h>
#include <commands/mkdavdircommandentity.h>
#include <commands/davrmcommandentity.h>
#include <commands/davcopycommandentity.h>
#include <commands/davmovecommandentity.h>
#include <commands/davlistcommandentity.h>
#include <commands/stdfunctioncommandentity.h>

#include <util/filepathutil.h>
#include <util/shellcommand.h>

OwnCloudBrowser::OwnCloudBrowser(QObject *parent, NextcloudSettingsBase *settings) : QObject(parent)
{
    this->webdav = 0;
    this->abortIntended = false;
    setSettings(settings);
    resetWebdav();
}

OwnCloudBrowser::~OwnCloudBrowser()
{
    parser.abort();
    if (webdav)
        delete webdav;
}

QWebdav* OwnCloudBrowser::getWebdav()
{
    return webdav;
}

void OwnCloudBrowser::resetWebdav()
{
    if (webdav) {
        disconnect(webdav, &QWebdav::errorChanged, this, &OwnCloudBrowser::proxyHandleLoginFailed);
        disconnect(&parser, 0, 0, 0);
        delete webdav;
    }
    webdav = new QWebdav();
    connect(webdav, &QWebdav::errorChanged, this, &OwnCloudBrowser::proxyHandleLoginFailed, Qt::DirectConnection);
    connect(&parser, &QWebdavDirParser::errorChanged, this, &OwnCloudBrowser::proxyHandleLoginFailed, Qt::DirectConnection);
}

void OwnCloudBrowser::reloadSettings()
{
    if (!this->settings || !this->webdav)
        return;

    applySettingsToWebdav(this->settings, webdav);
}

void OwnCloudBrowser::testConnection()
{
    connect(webdav, &QWebdav::checkSslCertifcate, this, &OwnCloudBrowser::proxyHandleSslError);
    connect(webdav, &QNetworkAccessManager::finished, this, &OwnCloudBrowser::testConnectionFinished, Qt::DirectConnection);

    parser.listDirectory(webdav, "/");
}

void OwnCloudBrowser::testConnectionFinished()
{
    qDebug() << "BEIDL Finished";

    disconnect(webdav, &QWebdav::checkSslCertifcate, this, &OwnCloudBrowser::proxyHandleSslError);
    disconnect(webdav, &QNetworkAccessManager::finished, this, &OwnCloudBrowser::testConnectionFinished);

    connect(&parser, &QWebdavDirParser::finished, this, &OwnCloudBrowser::handleResponse);
    emit loginSucceeded();
}

void OwnCloudBrowser::proxyHandleSslError(const QList<QSslError>& errors)
{
    QSslCertificate cert = errors[0].certificate();
    emit sslCertifcateError(webdav->digestToHex(cert.digest(QCryptographicHash::Md5)),
                            webdav->digestToHex(cert.digest(QCryptographicHash::Sha1)));
}

void OwnCloudBrowser::proxyHandleLoginFailed()
{
    if (!abortIntended) {
        qDebug() << "BEIDL Failed";
        disconnect(&parser, &QWebdavDirParser::finished, this, &OwnCloudBrowser::handleResponse);

        emit loginFailed();
    } else {
        abortIntended = false;
    }
}

void OwnCloudBrowser::handleResponse()
{
}

QString OwnCloudBrowser::getCanonicalPath(const QString &path)
{
    const QString token = QStringLiteral("..");
    const QString slash = QStringLiteral("/");
    const QStringList dirs = path.split(slash, QString::SkipEmptyParts);

    QStringList newDirs;
    for (const QString &dir : dirs) {
        if (dir == token)
            newDirs.pop_back();
        else
            newDirs.push_back(dir);
    }

    QString ret = slash;
    if (newDirs.length() > 0)
        ret += newDirs.join(slash) + slash;

    return ret;
}

NextcloudSettingsBase* OwnCloudBrowser::getSettings()
{
    return this->settings;
}

void OwnCloudBrowser::setSettings(NextcloudSettingsBase* settings)
{
    if (this->settings != settings) {
        if (this->settings)
            disconnect(this->settings);

        this->settings = settings;

        if (this->settings) {
            connect(this->settings, &NextcloudSettingsBase::settingsChanged,
                    this, &OwnCloudBrowser::reloadSettings);
            this->settings->readSettings();
        }

        emit settingsObjChanged();
    }
}

QString OwnCloudBrowser::getCurrentPath()
{
    return currentPath;
}

void OwnCloudBrowser::getDirectoryContent(QString path)
{
    currentPath = path;
    parser.listDirectory(webdav, path);
}

void OwnCloudBrowser::refreshDirectoryContent()
{
}

CommandEntity* OwnCloudBrowser::makeDirectoryRequest(QString dirName)
{
    MkDavDirCommandEntity* command =
            new MkDavDirCommandEntity(this, currentPath + dirName, this->webdav);

    emit refreshStarted(currentPath);

    return command;
}

CommandEntity* OwnCloudBrowser::removeRequest(QString name, bool refresh)
{
    DavRmCommandEntity* command =
            new DavRmCommandEntity(this, currentPath + name, this->webdav);

    if (refresh)
        emit refreshStarted(currentPath);

    return command;
}

CommandEntity* OwnCloudBrowser::moveRequest(QString from, QString to, bool refresh)
{
    DavMoveCommandEntity* command =
            new DavMoveCommandEntity(this, from, to, this->webdav);

    if (refresh)
        emit refreshStarted(currentPath);

    return command;
}

CommandEntity* OwnCloudBrowser::copyRequest(QString from, QString to, bool refresh)
{
    DavCopyCommandEntity* command =
            new DavCopyCommandEntity(this, from, to, this->webdav);

    if (refresh)
        emit refreshStarted(currentPath);

    return command;
}

CommandEntity* OwnCloudBrowser::directoryListingRequest(QString path)
{
    qDebug() << Q_FUNC_INFO;
    DavListCommandEntity* command =
            new DavListCommandEntity(this, path, this->webdav);

    return command;
}

QList<CommandEntity*> OwnCloudBrowser::fileDownloadRequest(QString remotePath, QString mimeType, bool open)
{
    QList<CommandEntity*> commands;

    FileDownloadCommandEntity* downloadCommand = Q_NULLPTR;

    QString name = remotePath.mid(remotePath.lastIndexOf("/") + 1);
    QString destination = FilePathUtil::destinationFromMIME(mimeType) + "/" + name;

    downloadCommand = new FileDownloadCommandEntity(this, remotePath, destination, this->webdav);
    commands.append(downloadCommand);

    if (open) {
        StdFunctionCommandEntity* executeCommand = new StdFunctionCommandEntity(this, [destination]() {
            ShellCommand::runCommand(QStringLiteral("xdg-open"), QStringList() << destination);
        });
        commands.append(executeCommand);
    }

    return commands;
}

QList<CommandEntity*> OwnCloudBrowser::fileUploadRequest(QString localPath, QString remotePath)
{
    qDebug() << "upload requested";

    QList<CommandEntity*> commands;

    FileUploadCommandEntity* uploadCommand =
            new FileUploadCommandEntity(this, localPath, remotePath, this->webdav);
    DavListCommandEntity* listCommand =
            new DavListCommandEntity(this, remotePath, this->webdav);

    commands.append(uploadCommand);
    commands.append(listCommand);
    return commands;
}
