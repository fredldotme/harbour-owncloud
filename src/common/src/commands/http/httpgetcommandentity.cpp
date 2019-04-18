#include "httpgetcommandentity.h"

#include <QAuthenticator>

HttpGetCommandEntity::HttpGetCommandEntity(QObject *parent,
                                           QString path,
                                           QMap<QByteArray, QByteArray> headers,
                                           AccountBase* settings) :
    HttpCommandEntity(parent, path, headers, settings) { }

bool HttpGetCommandEntity::startWork()
{
    if (!HttpCommandEntity::startWork())
        return false;

    qDebug() << "GET request:" << this->m_requestUrl.toString();
    this->m_reply = this->m_accessManager.get(this->m_request);

    QObject::connect(this->m_reply, &QNetworkReply::finished, this, [=]() {
        if (!this->m_reply) {
            qWarning() << "Invalid reply, aborting.";
            abortWork();
            return;
        }

        if (this->m_reply->error() != QNetworkReply::NoError) {
            qWarning() << "Error occured during HTTP get request:" << this->m_reply->errorString();
            abortWork();
            return;
        }

        const int httpStatusCode =
                this->m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (httpStatusCode < 200 || httpStatusCode >= 400) {
            qWarning() << "Invalid HTTP status code" << httpStatusCode;
            abortWork();
            return;
        }

        QVariantMap result;
        const QByteArray replyData = this->m_reply->readAll();
        const QString mimeType = this->m_reply->header(QNetworkRequest::ContentTypeHeader).toString();
        const int contentLength = this->m_reply->header(QNetworkRequest::ContentLengthHeader).toInt();
        result.insert("contentType", mimeType);
        result.insert("contentLength", contentLength);
        result.insert("content", replyData);
        result.insert("statusCode", httpStatusCode);
        this->m_resultData = result;

        Q_EMIT contentReady();

        qInfo() << "Request finished, status code" << httpStatusCode;
        qDebug() << "Response length:" << contentLength;

        setState(FINISHED);
        Q_EMIT done();
    });

    setState(RUNNING);
    return true;
}
