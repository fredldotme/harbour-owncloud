#include "fileuploadcommandentity.h"

#include <net/webdav_utils.h>

FileUploadCommandEntity::FileUploadCommandEntity(QObject* parent,
                                                 QString localPath,
                                                 QString remotePath,
                                                 QWebdav* client,
                                                 NextcloudSettingsBase* settings) :
    WebDavCommandEntity(parent, client, settings)
{
    this->m_localFile = new QFile(localPath, this);
    const QString fileName = QFileInfo(*this->m_localFile).fileName();
    this->m_remotePath = remotePath + fileName;

    // extensible list of command properties
    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("fileUpload");
    info["localPath"] = localPath;
    info["remotePath"] = remotePath;
    info["fileName"] = fileName;
    info["remoteFile"] = remotePath + fileName;
    this->m_commandInfo = CommandEntityInfo(info);
}

bool FileUploadCommandEntity::startWork()
{
    if (!CommandEntity::startWork())
        return false;

    if (!this->m_localFile->exists()) {
        qWarning() << "Local file" << this->m_localFile->fileName() << "does not exist, aborting.";
        abortWork();
        return false;
    }

    const bool isOpen = this->m_localFile->open(QFile::ReadOnly);
    if (!isOpen) {
        qWarning() << "Failed to open" << this->m_localFile->fileName() << ", aborting.";
        abortWork();
        return false;
    }

    this->m_reply = this->m_client->put(this->m_remotePath, this->m_localFile);

    const bool canStart = WebDavCommandEntity::startWork();
    if (!canStart)
        return false;

    setState(RUNNING);
    return true;
}

void FileUploadCommandEntity::setModifiedTime()
{
    QWebdav::PropValues props;
    QMap<QString, QVariant> propMap;

    QFileInfo localFileInfo(*this->m_localFile);
    qint64 lastModified = localFileInfo.lastModified().toMSecsSinceEpoch() / 1000; // seconds

    propMap["lastmodified"] = (QVariant) lastModified;
    props["DAV:"] = propMap;

    if (this->m_reply)
        this->m_reply->deleteLater();

    this->m_reply = this->m_client->proppatch(this->m_remotePath, props);
    QObject::connect(this->m_reply,
                     static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                     this, [=](QNetworkReply::NetworkError error) {
        qWarning() << "Failed to apply mtime" << lastModified << "due to error" << error << "continuing.";
        abortWork();
    });

    QObject::connect(this->m_reply, &QNetworkReply::finished, this, [=]() {
        if (this->m_reply->error() != QNetworkReply::NoError)
            return;

        Q_EMIT done();
    });
}
