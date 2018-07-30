#ifndef WEBDAV_UTILS_H
#define WEBDAV_UTILS_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include <qwebdav.h>
#include <settings/nextcloudsettingsbase.h>

QWebdav* getNewWebDav(NextcloudSettingsBase *settings,
                      QString apiPath = QStringLiteral(""),
                      QObject* parent = Q_NULLPTR);

void applySettingsToWebdav(NextcloudSettingsBase *settings,
                           QWebdav *webdav,
                           QString apiPath = QStringLiteral(""));

QMap<QByteArray, QByteArray> prepareOcsHeaders(
        NextcloudSettingsBase* settings = Q_NULLPTR,
        QMap<QByteArray, QByteArray> headers = QMap<QByteArray, QByteArray>());

QNetworkRequest getOcsRequest(const QNetworkRequest& request,
                              NextcloudSettingsBase* settings);


#endif // WEBDAV_UTILS_H

