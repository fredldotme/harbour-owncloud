#ifndef WEBDAV_UTILS_H
#define WEBDAV_UTILS_H

#include <QObject>
#include <QNetworkAccessManager>

#include <qwebdav.h>
#include <settings/nextcloudsettingsbase.h>

QWebdav* getNewWebDav(NextcloudSettingsBase *settings,
                      QString apiPath = QStringLiteral(""),
                      QObject* parent = Q_NULLPTR);
void applySettingsToWebdav(NextcloudSettingsBase *settings,
                           QWebdav *webdav,
                           QString apiPath = QStringLiteral(""));

#endif // WEBDAV_UTILS_H

