#include "fileuploadcommandentity.h"

#ifdef Q_OS_IOS
#include <QUrlQuery>
#endif

FileUploadCommandEntity::FileUploadCommandEntity(QObject* parent,
                                                 QString localPath,
                                                 QString remotePath,
                                                 QWebdav* client) :
    WebDavCommandEntity(parent, client),
    m_localFile(new QFile(localPath, this))
{
#ifdef Q_OS_IOS
    // On iOS we need to resolve the URL before use
    const QUrl localUrl(localPath);
    localPath = localUrl.toLocalFile();

    const QUrlQuery fileNameQuery(localPath.mid(localPath.indexOf("id=")));
    qInfo() << "FILENAME on iOS:" << fileNameQuery.toString(QUrl::FullyEncoded);
    const QString fileId = fileNameQuery.queryItemValue("id");
    const QString fileExtension = fileNameQuery.queryItemValue("ext");
    const QString fileName = fileId + "." + fileExtension;
#else
    const QString fileName = QFileInfo(*this->m_localFile).fileName();
#endif

    this->m_remotePath = remotePath + fileName;

    // extensible list of command properties
    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("davPut");
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

    if (!this->m_client) {
        qWarning() << "No valid client object available, aborting";
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
