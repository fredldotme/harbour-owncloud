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

QMap<QByteArray, QByteArray> prepareOcsHeaders(
         NextcloudSettingsBase* settings, QMap<QByteArray, QByteArray> headers)
{
    // Return current headers when no valid settings have been provided
    if (!settings)
        return headers;

    const QString username = settings ? settings->username() : QStringLiteral("");
    const QString password = settings ? settings->password() : QStringLiteral("");
    const QByteArray authorization =
            QStringLiteral("%1:%2").arg(username,
                                        password).toUtf8().toBase64();

    headers.insert(QByteArrayLiteral("Authorization"),
                   QStringLiteral("Basic %1").arg(QString(authorization)).toUtf8());
    headers.insert(QByteArrayLiteral("OCS-APIREQUEST"),
                   QByteArrayLiteral("true"));

    return headers;
}

QNetworkRequest getOcsRequest(const QNetworkRequest& request,
                              NextcloudSettingsBase* settings)
{
    qDebug() << Q_FUNC_INFO;

    // Read raw headers out of the provided request
    QMap<QByteArray, QByteArray> rawHeaders;
    for (const QByteArray& headerKey : request.rawHeaderList()) {
        rawHeaders.insert(headerKey, request.rawHeader(headerKey));
    }

    // Add OCS required headers
    rawHeaders = prepareOcsHeaders(settings, rawHeaders);

    // Construct new QNetworkRequest with prepared header values
    QNetworkRequest newRequest(request);
    for (const QByteArray& headerKey : request.rawHeaderList()) {
        newRequest.setRawHeader(headerKey, rawHeaders[headerKey]);
    }

    return newRequest;
}
