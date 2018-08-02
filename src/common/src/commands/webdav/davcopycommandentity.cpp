#include "davcopycommandentity.h"

DavCopyCommandEntity::DavCopyCommandEntity(QObject* parent,
                                           QString fromPath,
                                           QString toPath,
                                           QWebdav* client) :
    WebDavCommandEntity(parent, client)
{
    this->m_fromPath = fromPath;
    this->m_toPath = toPath;

    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("davCopy");
    info["fromPath"] = fromPath;
    info["toPath"] = toPath;
    this->m_commandInfo = CommandEntityInfo(info);
}

bool DavCopyCommandEntity::startWork()
{
    if (!this->m_client) {
        qWarning() << "No valid client object available, aborting";
        abortWork();
        return false;
    }
    this->m_reply = this->m_client->copy(this->m_fromPath, this->m_toPath);

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]() {
        qInfo() << "Dav entity copying" << this->m_fromPath << "to" << this->m_toPath << "complete.";
    });

    const bool canStart = WebDavCommandEntity::startWork();
    if (!canStart)
        return false;

    setState(RUNNING);
    return true;
}
