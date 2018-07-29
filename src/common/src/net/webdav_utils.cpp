#include "webdav_utils.h"

QWebdav* getNewWebDav(NextcloudSettingsBase *settings, QString apiPath, QObject* parent)
{
    // Allocating QWebdav object without settings doesn't make sense.
    if (!settings)
        return Q_NULLPTR;

    QWebdav* newWebdav = new QWebdav(parent);
    applySettingsToWebdav(settings, newWebdav, apiPath);
    qDebug() << "Returning webdav for host" << settings->hostname();

    return newWebdav;
}

void applySettingsToWebdav(NextcloudSettingsBase *settings, QWebdav *webdav, QString apiPath)
{
    if (!settings || !webdav)
        return;

    webdav->setConnectionSettings(settings->isHttps() ? QWebdav::HTTPS : QWebdav::HTTP,
                                  settings->hostname(),
                                  settings->path() + apiPath,
                                  settings->username(),
                                  settings->password(),
                                  settings->port(),
                                  settings->isHttps() ? settings->md5Hex() : "",
                                  settings->isHttps() ? settings->sha1Hex() : "");
}

QNetworkRequest getOcsRequest(const QNetworkRequest& request,
                              NextcloudSettingsBase* settings)
{
    qDebug() << Q_FUNC_INFO;

    QNetworkRequest ret(request);
    const QString username = settings ? settings->username() : QStringLiteral("");
    const QString password = settings ? settings->password() : QStringLiteral("");
    const QByteArray authorization =
            QStringLiteral("%1:%2").arg(username,
                                        password).toUtf8().toBase64();
    qDebug() << "Authorization:" << authorization;
    ret.setRawHeader(QByteArrayLiteral("Authorization"),
                     QStringLiteral("Basic %1").arg(QString(authorization)).toUtf8());

    ret.setRawHeader(QByteArrayLiteral("OCS-APIREQUEST"),
                     QByteArrayLiteral("true"));
    return ret;
}
