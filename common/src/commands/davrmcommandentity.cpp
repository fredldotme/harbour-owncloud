#include "davrmcommandentity.h"

DavRmCommandEntity::DavRmCommandEntity(QObject *parent,
                                       QString remotePath,
                                       QWebdav* client,
                                       NextcloudSettingsBase* settings) :
    WebDavCommandEntity(parent, client, settings)
{
    this->m_remotePath = remotePath;
}

void DavRmCommandEntity::startWork()
{
    this->m_reply = this->m_client->remove(this->m_remotePath);

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]() {
        qInfo() << "Dav entity removal" << this->m_remotePath << "complete.";
    });

    setState(RUNNING);
    WebDavCommandEntity::startWork();
}
