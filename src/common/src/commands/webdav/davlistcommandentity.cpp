#include "davlistcommandentity.h"

const QString getDirNameFromPath(const QString& path)
{
    const QString separator = QStringLiteral("/");
    if (!path.contains(separator))
        return path;
    const QStringList splitPath = path.split(separator, QString::SkipEmptyParts);
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
        qWarning() << errorStr;
        abortWork();
    });

    QObject::connect(&this->m_parser, &QWebdavDirParser::finished, this, [=]() {
        qInfo() << "Listing remote directory content" << this->m_remotePath << "complete.";
        qInfo() << "DIRECTORY SIZE" << this->m_parser.getList().size();

        QVariantList result;

        for(const QWebdavItem& item : this->m_parser.getList()) {
            QVariantMap info;
            info.insert("path", item.path());
            info.insert("name", item.name());
            info.insert("isDirectory", item.isDir());
            info.insert("size", item.size());
            info.insert("createdAt", item.createdAt());
            info.insert("entityTag", item.entityTag());
            info.insert("fileId", item.fileId());
            if(!item.isDir()) {
                info.insert("isExecutable", item.isExecutable());
                info.insert("mimeType", item.mimeType());
                info.insert("lastModified", item.lastModified());
            }
            result.append(info);
        }
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
