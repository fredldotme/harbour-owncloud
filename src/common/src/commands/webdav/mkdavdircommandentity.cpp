#include "mkdavdircommandentity.h"
#include <net/webdav_utils.h>


MkDavDirCommandEntity::MkDavDirCommandEntity(QObject* parent,
                                             QString remotePath,
                                             QWebdav* client,
                                             NextcloudSettingsBase* settings) :
    WebDavCommandEntity(parent, client, settings)
{
    this->m_remotePath = remotePath;

    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("davMkDir");
    info["remotePath"] = remotePath;
    this->m_commandInfo = CommandEntityInfo(info);
}

bool MkDavDirCommandEntity::startWork()
{
    this->m_reply = this->m_client->mkdir(this->m_remotePath);

    QObject::connect(m_reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this,
                     [=](QNetworkReply::NetworkError error) {
        qInfo() << "Remote directory creation" << this->m_remotePath << "failed,"
                << "error:" << error;
    });

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]() {
        qInfo() << "Remote directory creation" << this->m_remotePath << "complete.";
    });

    const bool canStart = WebDavCommandEntity::startWork();
    if (!canStart)
        return false;

    setState(RUNNING);
    return true;
}
