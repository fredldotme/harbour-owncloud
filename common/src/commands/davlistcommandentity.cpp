#include "davlistcommandentity.h"

DavListCommandEntity::DavListCommandEntity(QObject *parent,
                                           QString remotePath,
                                           QWebdav* client,
                                           NextcloudSettingsBase* settings) :
    WebDavCommandEntity(parent, client, settings)
{
    this->m_remotePath = remotePath;

    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("davList");
    info["remotePath"] = remotePath;
    this->m_commandInfo = CommandEntityInfo(info);
}

void DavListCommandEntity::startWork()
{
    qDebug() << Q_FUNC_INFO;
    this->m_parser.listDirectory(this->m_client, this->m_remotePath);

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
            if(!item.isDir()) {
                info.insert("mimeType", item.mimeType());
                info.insert("lastModified", item.lastModified());
            }
            result.append(info);
        }
        this->m_resultData = result;
        Q_EMIT done();
    });

    setState(RUNNING);
    WebDavCommandEntity::startWork();
}
