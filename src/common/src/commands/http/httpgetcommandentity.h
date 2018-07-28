#ifndef HTTPGETCOMMANDENTITY_H
#define HTTPGETCOMMANDENTITY_H

#include <QObject>
#include <commands/http/httpcommandentity.h>
#include <settings/nextcloudsettingsbase.h>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class HttpGetCommandEntity : public HttpCommandEntity
{
    Q_OBJECT
public:
    explicit HttpGetCommandEntity(QObject* parent = Q_NULLPTR,
                                  QString path = QStringLiteral(""),
                                  QMap<QByteArray, QByteArray> headers = QMap<QByteArray, QByteArray>(),
                                  NextcloudSettingsBase* settings = Q_NULLPTR);

    bool startWork();

signals:
    void contentReady();

};

#endif // HTTPGETCOMMANDENTITY_H
