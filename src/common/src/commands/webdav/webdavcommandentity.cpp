#include "webdavcommandentity.h"

#include <net/webdav_utils.h>

WebDavCommandEntity::WebDavCommandEntity(QObject* parent,
                                         QWebdav* client,
                                         NextcloudSettingsBase* settings) :
    CommandEntity(parent), m_client(client), m_settings(settings)
{
}

WebDavCommandEntity::~WebDavCommandEntity()
{
    if (this->m_reply) {
        this->m_reply->deleteLater();
        this->m_reply = Q_NULLPTR;
    }
}

bool WebDavCommandEntity::startWork()
{
    if (!CommandEntity::startWork()) {
        abortWork();
        return false;
    }

    if (this->m_reply) {
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
            const qreal newProgress = ((qreal)bytesReceived/(qreal)bytesTotal);
            setProgress(newProgress);
        });
        QObject::connect(this->m_reply, &QNetworkReply::uploadProgress,
                         this, [=](qint64 bytesSent, qint64 bytesTotal) {
            if (bytesTotal < 1)
                return;
            const qreal newProgress = ((qreal)bytesSent/(qreal)bytesTotal);
            setProgress(newProgress);
        });
    }

    if (!this->m_client) {
        qWarning() << "No valid client object available, aborting";
        abortWork();
        return false;
    }

    QObject::connect(this->m_client, &QWebdav::checkSslCertifcate,
                     this, [=](const QList<QSslError> &errors) {
        qWarning() << "SSL error occured";

        if (errors.length() > 0) {
            QSslCertificate sslcert = errors[0].certificate();
            const QString md5Digest = sslcert.digest(QCryptographicHash::Md5);
            const QString sha1Digest = sslcert.digest(QCryptographicHash::Sha1);

            Q_EMIT sslErrorOccured(md5Digest, sha1Digest);
        }
        abortWork();
    });

    return true;
}

bool WebDavCommandEntity::abortWork()
{
    qDebug() << Q_FUNC_INFO;
    if (!CommandEntity::abortWork())
        return false;

    if (this->m_reply) {
        if (!this->m_reply->isFinished())
            this->m_reply->abort();
        this->m_reply->deleteLater();
        this->m_reply = Q_NULLPTR;
    }

    setState(ABORTED);
    Q_EMIT aborted();
    return true;
}
