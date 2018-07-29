#include "ocsnetaccessfactory.h"

#include <QDebug>
#include <QNetworkDiskCache>
#include <QStandardPaths>
#include <net/webdav_utils.h>

OscNetAccess::OscNetAccess(QObject* parent, NextcloudSettingsBase* settings) :
    QNetworkAccessManager(parent), m_settings(settings)
{ }

QNetworkReply* OscNetAccess::createRequest(Operation op,
                                           const QNetworkRequest &request,
                                           QIODevice *outgoingData)
{
    qDebug() << Q_FUNC_INFO << this->m_settings;
    return QNetworkAccessManager::createRequest(op,
                                                getOcsRequest(request, this->m_settings),
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
