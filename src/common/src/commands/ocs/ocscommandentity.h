#ifndef OCSCOMMANDENTITY_H
#define OCSCOMMANDENTITY_H

#include <QObject>
#include <commands/http/httpgetcommandentity.h>
#include <QMap>
#include <QByteArray>

class OcsCommandEntity : public HttpGetCommandEntity
{
    Q_OBJECT

public:
    static QMap<QByteArray, QByteArray> prepareOcsHeaders(
            QMap<QByteArray, QByteArray> headers = QMap<QByteArray, QByteArray>(),
            NextcloudSettingsBase* settings = Q_NULLPTR);

    explicit OcsCommandEntity(
            QObject* parent = Q_NULLPTR,
            QString path = QStringLiteral(""),
            QMap<QByteArray, QByteArray> headers = prepareOcsHeaders(),
            NextcloudSettingsBase* settings = Q_NULLPTR);

};

#endif // OCSCOMMANDENTITY_H
