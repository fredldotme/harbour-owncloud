#include "ocscommandentity.h"

QMap<QByteArray, QByteArray> OcsCommandEntity::prepareOcsHeaders(
        QMap<QByteArray, QByteArray> headers, NextcloudSettingsBase* settings)
{
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

OcsCommandEntity::OcsCommandEntity(QObject *parent,
                                   QString path,
                                   QMap<QByteArray, QByteArray> headers,
                                   NextcloudSettingsBase* settings) :
    HttpGetCommandEntity(parent, path, prepareOcsHeaders(headers, settings), settings)
{

}
