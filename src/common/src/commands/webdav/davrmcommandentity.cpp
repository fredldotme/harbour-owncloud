#include "davrmcommandentity.h"

DavRmCommandEntity::DavRmCommandEntity(QObject *parent,
                                       QString remotePath,
                                       QWebdav* client,
                                       NextcloudSettingsBase* settings) :
    WebDavCommandEntity(parent, client, settings)
{
    this->m_remotePath = remotePath;

    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("davRemove");
    info["remotePath"] = remotePath;
    this->m_commandInfo = CommandEntityInfo(info);
}

bool DavRmCommandEntity::startWork()
{
    if (!CommandEntity::startWork())
        return false;

    if (!this->m_client) {
        qWarning() << "No valid client object available, aborting";
        abortWork();
        return false;
    }

    this->m_reply = this->m_client->remove(this->m_remotePath);

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]() {
        qInfo() << "Dav entity removal" << this->m_remotePath << "complete.";
    });

    const bool canStart = WebDavCommandEntity::startWork();
    if (!canStart)
        return false;

    setState(RUNNING);
    return true;
}
