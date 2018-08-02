#include "filedownloadcommandentity.h"

#include <net/webdav_utils.h>
#include <qwebdavitem.h>

FileDownloadCommandEntity::FileDownloadCommandEntity(QObject* parent,
                                                     QString remotePath,
                                                     QString localPath,
                                                     QWebdav* client) :
    WebDavCommandEntity(parent, client)
{
    this->m_remotePath = remotePath;
    this->m_localFile = new QFile(localPath, this);
    const QString localDir = localPath.left(localPath.lastIndexOf(QDir::separator())+1);
    this->m_localDir = QDir(localDir);
    const QString fileName = QFileInfo(*this->m_localFile).fileName();

    // extensible list of command properties
    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("davGet");
    info["localPath"] = localPath;
    info["remotePath"] = remotePath;
    info["fileName"] = fileName;
    info["remoteFile"] = remotePath + fileName;
    this->m_commandInfo = CommandEntityInfo(info);
}

bool FileDownloadCommandEntity::startWork()
{
    if (!CommandEntity::startWork())
        return false;

    setState(RUNNING);
    if (!this->m_localDir.exists()) {
        const bool mkPathSuccess = this->m_localDir.mkpath(this->m_localDir.absolutePath());
        if (!mkPathSuccess) {
            qWarning() << "Failed to create target path" << this->m_localDir.absolutePath();
            abortWork();
            return false;
        }
    }

    // Remove existing file before download
    if (this->m_localFile->exists()) {
        const bool removeSuccess = this->m_localFile->remove();
        if (!removeSuccess) {
            qWarning() << "Failed to remove existing file" << this->m_localFile->fileName() << ", aborting.";
            abortWork();
            return false;
        }
    }

    const bool isOpen = this->m_localFile->open(QFile::ReadWrite);
    if (!isOpen) {
        qWarning() << "Failed to create local file, aborting.";
        abortWork();
        return false;
    }

    if (!this->m_client) {
        qWarning() << "No valid client object available, aborting";
        abortWork();
        return false;
    }

    this->m_reply = this->m_client->get(this->m_remotePath, this->m_localFile);
    QObject::connect(this->m_reply, &QNetworkReply::finished, this, [=]() {
        qInfo() << "File download" << this->m_remotePath << "complete.";
    });

    const bool canStart = WebDavCommandEntity::startWork();
    if (!canStart) {
        return false;
    }

    setState(RUNNING);
    return true;
}

bool FileDownloadCommandEntity::abortWork()
{
    const bool success = WebDavCommandEntity::abortWork();

    // Don't keep stale files around in case of command abortion
    if (this->m_localFile && this->m_localFile->exists()) {
        this->m_localFile->remove();
    }

    return success;
}
