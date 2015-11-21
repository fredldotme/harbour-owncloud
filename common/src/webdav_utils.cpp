#include "webdav_utils.h"

QWebdav* getNewWebDav(Settings *settings)
{
    /* Used for file uploads
     * Helps to not confuse error signals of simultaneous file operations */
    QWebdav* newWebdav = new QWebdav();
    QObject::connect(newWebdav, &QNetworkAccessManager::finished, newWebdav, &QObject::deleteLater, Qt::DirectConnection);

    applySettingsToWebdav(settings, newWebdav);
    qDebug() << "Returning webdav for host" << settings->hostname();
    return newWebdav;
}

void applySettingsToWebdav(Settings *settings, QWebdav *webdav)
{
    webdav->setConnectionSettings(settings->isHttps() ? QWebdav::HTTPS : QWebdav::HTTP,
                                 settings->hostname(),
                                 settings->path() + "remote.php/webdav",
                                 settings->username(),
                                 settings->password(),
                                 settings->port(),
                                 settings->isHttps() ? settings->md5Hex() : "",
                                 settings->isHttps() ? settings->sha1Hex() : "");
}


