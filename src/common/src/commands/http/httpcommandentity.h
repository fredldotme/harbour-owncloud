#ifndef HTTPCOMMANDENTITY_H
#define HTTPCOMMANDENTITY_H

#include <QObject>
#include <QMap>
#include <commandentity.h>
#include <settings/nextcloudsettingsbase.h>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class HttpCommandEntity : public CommandEntity
{
    Q_OBJECT
public:
    explicit HttpCommandEntity(QObject *parent = Q_NULLPTR,
                               QString path = QStringLiteral(""),
                               QMap<QByteArray, QByteArray> headers = QMap<QByteArray, QByteArray>(),
                               AccountBase* settings = Q_NULLPTR);
    ~HttpCommandEntity();
    bool startWork();
    bool abortWork();

protected:
    QUrl m_requestUrl;
    QNetworkReply* m_reply = Q_NULLPTR;
    QNetworkRequest m_request;
    QNetworkAccessManager m_accessManager;
    AccountBase* m_settings = Q_NULLPTR;

private:
    QUrl setupRequestUrl();

    QString m_path;
    QMap<QByteArray, QByteArray> m_headers;

signals:
    void sslErrorOccured(QString md5Digest, QString sha1Digest);

};

#endif // HTTPCOMMANDENTITY_H
