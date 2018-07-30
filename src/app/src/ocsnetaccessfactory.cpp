#include "ocsnetaccessfactory.h"

#include <QDebug>
#include <QNetworkDiskCache>
#include <QStandardPaths>
#include <net/webdav_utils.h>

OscNetAccess::OscNetAccess(QObject* parent, NextcloudSettingsBase* settings) :
    QNetworkAccessManager(parent), m_settings(settings)
{
    QObject::connect(this, &OscNetAccess::sslErrors,
                     this, [=](QNetworkReply* reply, const QList<QSslError> &errors) {
        if (errors.length() > 0) {
            QSslCertificate sslcert = errors[0].certificate();
            const QString md5Digest = sslcert.digest(QCryptographicHash::Md5);
            const QString sha1Digest = sslcert.digest(QCryptographicHash::Sha1);

            if (md5Digest == hexToDigest(this->m_settings->md5Hex()) &&
                    sha1Digest == hexToDigest(this->m_settings->sha1Hex())) {
                reply->ignoreSslErrors();

                return;
            }
        }

        qWarning() << "OscNetAccess: unhandled SSL error occured";
    });}

QNetworkReply* OscNetAccess::createRequest(Operation op,
                                           const QNetworkRequest &request,
                                           QIODevice *outgoingData)
{
    // Create QNetworkRequest with proper authorization headers
    // if a settings object has been provided at construction time.
    const QNetworkRequest actualRequest = this->m_settings ?
                getOcsRequest(request, this->m_settings) :
                request;

    return QNetworkAccessManager::createRequest(op,
                                                actualRequest,
                                                outgoingData);
}

OcsNetAccessFactory::OcsNetAccessFactory(NextcloudSettingsBase *settings)
    : m_settings(settings)
{
    qDebug() << Q_FUNC_INFO;
}

OcsNetAccessFactory::OcsNetAccessFactory(const OcsNetAccessFactory &o)
{
    qDebug() << Q_FUNC_INFO;
    this->m_settings = o.m_settings;
}

QNetworkAccessManager* OcsNetAccessFactory::create(QObject *parent)
{
    qDebug() << Q_FUNC_INFO;

    OscNetAccess *nam = new OscNetAccess(parent, this->m_settings);
    QNetworkDiskCache* cache = new QNetworkDiskCache(nam);
    cache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    nam->setCache(cache);
    return nam;
}

/*QUrl OcsNetAccessFactory::intercept(const QUrl& url, DataType type)
{

    if (type != QQmlAbstractUrlInterceptor::UrlString)
        return url;

    qDebug() << Q_FUNC_INFO << url.scheme() << url;

    if (!(url.scheme().startsWith(QStringLiteral("http://")) ||
          url.scheme().startsWith(QStringLiteral("https://"))))
        return url;

    return url;
}*/
