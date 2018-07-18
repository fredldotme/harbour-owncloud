#include "remotedirectorycommandentity.h"
#include <net/webdav_utils.h>


RemoteDirectoryCommandEntity::RemoteDirectoryCommandEntity(QObject* parent,
                                                           QString remotePath,
                                                           NextcloudSettingsBase* settings) :
    CommandEntity(parent)
{
    this->m_remotePath = remotePath;
    this->m_client = getNewWebDav(settings);
}

RemoteDirectoryCommandEntity::~RemoteDirectoryCommandEntity()
{
    if (this->m_reply) {
        delete this->m_reply;
        this->m_reply = Q_NULLPTR;
    }

    if (this->m_client) {
        delete this->m_client;
        this->m_client = Q_NULLPTR;
    }
}

void RemoteDirectoryCommandEntity::startWork()
{
    this->m_reply = this->m_client->mkdir(this->m_remotePath);

    QObject::connect(m_reply,
                     static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this,
                     [=](QNetworkReply::NetworkError error) {
        qWarning() << "Aborting due to network error:" << error;
        abortWork();
    });
    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]() {
        if (this->m_reply->error() != QNetworkReply::NoError)
            return;

        qInfo() << "Remote directory creation" << this->m_remotePath << "complete.";
        Q_EMIT done();
    }, Qt::DirectConnection);

    setState(RUNNING);
}

void RemoteDirectoryCommandEntity::abortWork()
{
    if (this->m_reply) {
        this->m_reply->abort();
        delete this->m_reply;
        this->m_reply = Q_NULLPTR;
    }

    if (this->m_client) {
        delete this->m_client;
        this->m_client = Q_NULLPTR;
    }

    Q_EMIT aborted();
}
