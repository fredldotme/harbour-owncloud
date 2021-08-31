#include "davlistcommandentity.h"

const QString getDirNameFromPath(const QString& path)
{
    const QString separator = QStringLiteral("/");
    if (!path.contains(separator))
        return path;

#if QT_VERSION < 0x060000
    const QStringList splitPath = path.split(separator, QString::SkipEmptyParts);
#else
    const QStringList splitPath = path.split(separator, Qt::SkipEmptyParts);
#endif
    if (splitPath.length() < 1)
        return separator;
    return splitPath[splitPath.length()-1];
}

DavListCommandEntity::DavListCommandEntity(QObject *parent,
                                           QString remotePath,
                                           bool refresh,
                                           QWebdav* client) :
    WebDavCommandEntity(parent, client)
{
    this->m_remotePath = remotePath;

    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("davList");
    info["remotePath"] = remotePath;
    info["name"] = getDirNameFromPath(remotePath);
    info["refresh"] = refresh;
    this->m_commandInfo = CommandEntityInfo(info);
}

bool DavListCommandEntity::startWork()
{
    QObject::connect(&this->m_parser, &QWebdavDirParser::errorChanged,
                     this, [=](QString errorStr){
        qWarning() << "Error occured while parsing directory content for" << this->m_remotePath;
        qWarning() << this->m_parser.httpCode() << this->m_parser.error() << errorStr;
        this->m_parser.abort();
        Q_EMIT aborted();
    });

    QObject::connect(&this->m_parser, &QWebdavDirParser::finished, this, [=]() {
        qInfo() << "Listing remote directory content" << this->m_remotePath << "complete.";
        qInfo() << "DIRECTORY SIZE" << this->m_parser.getList().size();

        QVariantMap result;
        const int httpCode = this->m_parser.httpCode();
        const bool success = (httpCode >= 200 && httpCode < 300);

        result.insert(QStringLiteral("success"), success && (this->m_parser.error() == QNetworkReply::NoError));
        result.insert(QStringLiteral("httpCode"), httpCode);

        QVariantList directoryContent;

        for(const QWebdavItem& item : this->m_parser.getList()) {
            QVariantMap info;
            info.insert("path", item.path());
            info.insert("name", item.name());
            info.insert("isDirectory", item.isDir());
            info.insert("size", item.size());
            info.insert("createdAt", item.createdAt());
            info.insert("entityTag", item.entityTag());
            info.insert("uniqueId", item.entityTag());
            info.insert("fileId", item.fileId());
            info.insert("lastModified", item.lastModified());
            if(!item.isDir()) {
                info.insert("isExecutable", item.isExecutable());
                info.insert("mimeType", item.mimeType());
            }
            directoryContent.append(info);
        }
        result.insert(QStringLiteral("dirContent"), directoryContent);
        this->m_resultData = result;
        Q_EMIT done();
    });

    const bool canStart = WebDavCommandEntity::startWork();
    if (!canStart) {
        qWarning() << "Cannot startWork due to !WebDavCommandEntity::startWork()";
        return false;
    }

    qDebug() << Q_FUNC_INFO;
    this->m_parser.listDirectory(this->m_client, this->m_remotePath);

    setState(RUNNING);
    qDebug() << Q_FUNC_INFO << "done";
    return true;
}
