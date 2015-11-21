#ifndef WEBDAV_UTILS_H
#define WEBDAV_UTILS_H

#include <QObject>
#include <QNetworkAccessManager>

#include <qwebdav.h>
#include "settings.h"

QWebdav* getNewWebDav(Settings *settings);
void applySettingsToWebdav(Settings *settings, QWebdav *webdav);

#endif // WEBDAV_UTILS_H

