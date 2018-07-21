#include "mkdavdircommandentity.h"
#include <net/webdav_utils.h>


MkDavDirCommandEntity::MkDavDirCommandEntity(QObject* parent,
                                             QString remotePath,
                                             QWebdav* client,
                                             NextcloudSettingsBase* settings) :
    WebDavCommandEntity(parent, client, settings)
{
    this->m_remotePath = remotePath;
}

void MkDavDirCommandEntity::startWork()
{
    this->m_reply = this->m_client->mkdir(this->m_remotePath);

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]() {
        qInfo() << "Remote directory creation" << this->m_remotePath << "complete.";
    });

    setState(RUNNING);
    WebDavCommandEntity::startWork();
}
