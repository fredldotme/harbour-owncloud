#include "filedownloadcommandentity.h"

#include <net/webdav_utils.h>
#include <qwebdavitem.h>

FileDownloadCommandEntity::FileDownloadCommandEntity(QObject* parent,
                                                     QString remotePath,
                                                     QString localPath,
                                                     QWebdav* client,
                                                     NextcloudSettingsBase* settings) :
    WebDavCommandEntity(parent, client, settings)
{
    this->m_remotePath = remotePath;
    this->m_localFile = new QFile(localPath, this);
    const QString localDir = localPath.left(localPath.lastIndexOf(QDir::separator())+1);
    this->m_localDir = QDir(localDir);

    // extensible list of command properties
    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("fileDownload");
    info["localPath"] = localPath;
    info["remotePath"] = remotePath;
    info["fileName"] = QFileInfo(*this->m_localFile).fileName();
    this->m_commandInfo = CommandEntityInfo(info);
}

void FileDownloadCommandEntity::startWork()
{
    setState(RUNNING);
    if (!this->m_localDir.exists()) {
        this->m_localDir.mkpath(this->m_localDir.absolutePath());
    }

    // Remove existing file before download
    if (this->m_localFile->exists()) {
        const bool removeSuccess = this->m_localFile->remove();
        if (!removeSuccess) {
            qWarning() << "Failed to remove existing file" << this->m_localFile->fileName() << ", aborting.";
            abortWork();
            return;
        }
    }

    const bool isOpen = this->m_localFile->open(QFile::ReadWrite);
    if (!isOpen) {
        qWarning() << "Failed to create local file, aborting.";
        abortWork();
        return;
    }

    this->m_reply = this->m_client->get(this->m_remotePath, this->m_localFile);
    QObject::connect(this->m_reply, &QNetworkReply::finished, this, [=]() {
        qInfo() << "File download" << this->m_remotePath << "complete.";
    });

    WebDavCommandEntity::startWork();
}
