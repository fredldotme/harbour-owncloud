#include "davcopycommandentity.h"

DavCopyCommandEntity::DavCopyCommandEntity(QObject* parent,
                                           QString fromPath,
                                           QString toPath,
                                           QWebdav* client,
                                           NextcloudSettingsBase* settings) :
    WebDavCommandEntity(parent, client, settings)
{
    this->m_fromPath = fromPath;
    this->m_toPath = toPath;
}

void DavCopyCommandEntity::startWork()
{
    this->m_reply = this->m_client->copy(this->m_fromPath, this->m_toPath);

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]() {
        qInfo() << "Dav entity copying" << this->m_fromPath << "to" << this->m_toPath << "complete.";
    });

    setState(RUNNING);
    WebDavCommandEntity::startWork();
}
