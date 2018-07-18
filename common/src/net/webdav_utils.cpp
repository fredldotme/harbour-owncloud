#include "webdav_utils.h"

QWebdav* getNewWebDav(NextcloudSettingsBase *settings, QString apiPath, QObject* parent)
{
    /* Used for file uploads
     * Helps to not confuse error signals of simultaneous file operations */
    QWebdav* newWebdav = new QWebdav(parent);
    QObject::connect(newWebdav, &QNetworkAccessManager::finished, newWebdav, &QObject::deleteLater);

    applySettingsToWebdav(settings, newWebdav, apiPath);
    qDebug() << "Returning webdav for host" << settings->hostname();
    return newWebdav;
}

void applySettingsToWebdav(NextcloudSettingsBase *settings, QWebdav *webdav, QString apiPath)
{
    if (apiPath.isEmpty())
        apiPath = QStringLiteral("remote.php/webdav");

    webdav->setConnectionSettings(settings->isHttps() ? QWebdav::HTTPS : QWebdav::HTTP,
                                  settings->hostname(),
                                  settings->path() + apiPath,
                                  settings->username(),
                                  settings->password(),
                                  settings->port(),
                                  settings->isHttps() ? settings->md5Hex() : "",
                                  settings->isHttps() ? settings->sha1Hex() : "");
}


