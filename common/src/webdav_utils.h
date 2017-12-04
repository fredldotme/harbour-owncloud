#ifndef WEBDAV_UTILS_H
#define WEBDAV_UTILS_H

#include <QObject>
#include <QNetworkAccessManager>

#include <qwebdav.h>
#include <nextcloudsettingsbase.h>

QWebdav* getNewWebDav(NextcloudSettingsBase *settings);
void applySettingsToWebdav(NextcloudSettingsBase *settings, QWebdav *webdav);

#endif // WEBDAV_UTILS_H

