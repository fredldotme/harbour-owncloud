#include "httpcommandentity.h"
#include <QAuthenticator>

HttpCommandEntity::HttpCommandEntity(QObject *parent,
                                     QString path,
                                     QMap<QByteArray, QByteArray> headers,
                                     NextcloudSettingsBase* settings) :
  CommandEntity(parent), m_settings(settings), m_path(path), m_headers(headers) { }

HttpCommandEntity::~HttpCommandEntity()
{
    if (this->m_reply) {
        this->m_reply->deleteLater();
        this->m_reply = Q_NULLPTR;
    }
}

bool HttpCommandEntity::startWork()
{
    if (!CommandEntity::startWork())
        return false;

    if (!this->m_settings) {
        qWarning() << "no settings object provided, HTTP request aborted";
        abortWork();
        return false;
    }

    QObject::connect(&this->m_accessManager, &QNetworkAccessManager::authenticationRequired,
                     this, [=](QNetworkReply *reply, QAuthenticator *authenticator) {
        Q_UNUSED(reply)
        qDebug() << "Providing authenticator";

        if (!authenticator) {
            qWarning() << "Invalid authenticator received, aborting";
            abortWork();
            return;
        }

        authenticator->setUser(this->m_settings->username());
        authenticator->setPassword(this->m_settings->password());
    });

    QObject::connect(&this->m_accessManager, &QNetworkAccessManager::sslErrors,
                     this, [=](QNetworkReply *reply, const QList<QSslError> &errors){
        if (errors.length() < 1) {
            qWarning() << "List of errors is empty, aborting";
            abortWork();
            return;
        }

        QSslCertificate sslcert = errors[0].certificate();
        const QString md5Digest = sslcert.digest(QCryptographicHash::Md5);
        const QString sha1Digest = sslcert.digest(QCryptographicHash::Sha1);
        if (md5Digest == hexToDigest(this->m_settings->md5Hex()) &&
                sha1Digest == hexToDigest(this->m_settings->sha1Hex())) {
            // user accepted this SSL certifcate already ==> ignore SSL errors
            reply->ignoreSslErrors();
        } else {
            Q_EMIT sslErrorOccured(md5Digest, sha1Digest);
            qWarning() << "Invalid SSL certificate, aborting.";
            reply->abort();
            abortWork();
            return;
        }
    });

    if (this->m_settings->isHttps()) {
        this->m_accessManager.connectToHostEncrypted(this->m_settings->hostname(),
                                                     this->m_settings->port());
    } else {
        this->m_accessManager.connectToHost(this->m_settings->hostname(),
                                            this->m_settings->port());
    }

    this->m_requestUrl = setupRequestUrl();
    this->m_request.setUrl(this->m_requestUrl);
    this->m_request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    for (const QByteArray& key : this->m_headers.keys()) {
        const QByteArray value = this->m_headers.value(key);
        this->m_request.setRawHeader(key, value);
    }
    return true;
}

bool HttpCommandEntity::abortWork()
{
    if (!CommandEntity::abortWork())
        return false;

    if (this->m_reply) {
        this->m_reply->abort();
        this->m_reply->deleteLater();
        this->m_reply = Q_NULLPTR;
    }

    setState(ABORTED);
    Q_EMIT aborted();
    return true;
}

QByteArray HttpCommandEntity::hexToDigest(const QString &input)
{
    QByteArray result;
    int i = 2;
    int l = input.size();
    result.append(input.left(2).toLatin1());
    while ((i<l) && (input.at(i) == ':')) {
        ++i;
        result.append(input.mid(i,2).toLatin1());
        i+=2;
    }
    return QByteArray::fromHex(result);
}

QUrl HttpCommandEntity::setupRequestUrl()
{
    const QString pathToCloudInstall = this->m_settings ?
                this->m_settings->path() :
                QStringLiteral("/");
    QString urlPath = pathToCloudInstall + this->m_path;
    if (!urlPath.startsWith("/"))
        urlPath = QStringLiteral("/") + this->m_path;

    QUrl requestUrl;
    requestUrl.setScheme(this->m_settings->isHttps() ?
                             QStringLiteral("https") :
                             QStringLiteral("http"));
    requestUrl.setHost(this->m_settings->hostname());
    requestUrl.setPort(this->m_settings->port());
    requestUrl.setPath(urlPath);

    return requestUrl;
}
