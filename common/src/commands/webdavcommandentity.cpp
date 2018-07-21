#include "webdavcommandentity.h"

#include <net/webdav_utils.h>

WebDavCommandEntity::WebDavCommandEntity(QObject* parent,
                                         QWebdav* client,
                                         NextcloudSettingsBase* settings) :
    CommandEntity(parent)
{
    this->m_client = client ? client : getNewWebDav(settings);
}

WebDavCommandEntity::~WebDavCommandEntity()
{
    if (this->m_reply) {
        this->m_reply->deleteLater();
        this->m_reply = Q_NULLPTR;
    }
}

void WebDavCommandEntity::startWork()
{
    if (!this->m_reply)
        return;

    QObject::connect(this->m_reply,
                     static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this,
                     [=](QNetworkReply::NetworkError error) {
        qWarning() << "Aborting due to network error:" << error;
        abortWork();
    });

    QObject::connect(this->m_reply, &QNetworkReply::finished, this, [=]() {
        qDebug() << "WebDav request complete:" << this->m_reply->url().toString();
        qDebug() << this->m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (this->m_reply->error() != QNetworkReply::NoError)
            return;

        Q_EMIT done();
    });

    QObject::connect(this->m_reply, &QNetworkReply::downloadProgress,
                     this, [=](qint64 bytesReceived, qint64 bytesTotal) {
        if (bytesTotal < 1)
            return;
        setProgress(bytesReceived/bytesTotal);
    });
}

void WebDavCommandEntity::abortWork()
{
    if (this->m_reply) {
        this->m_reply->abort();
        this->m_reply->deleteLater();
        this->m_reply = Q_NULLPTR;
    }

    setState(ABORTED);
    Q_EMIT aborted();
}
