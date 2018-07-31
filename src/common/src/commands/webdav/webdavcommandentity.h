#ifndef WEBDAVCOMMANDENTITY_H
#define WEBDAVCOMMANDENTITY_H

#include <QObject>
#include <commandentity.h>
#include <settings/nextcloudsettingsbase.h>
#include <qwebdav.h>

class WebDavCommandEntity : public CommandEntity
{
    Q_OBJECT
public:
    explicit WebDavCommandEntity(QObject* parent = Q_NULLPTR,
                                 QWebdav* client = Q_NULLPTR,
                                 NextcloudSettingsBase* settings = Q_NULLPTR);
    ~WebDavCommandEntity();

protected:
    bool startWork() Q_DECL_OVERRIDE;
    bool abortWork() Q_DECL_OVERRIDE;

    QWebdav* m_client = Q_NULLPTR;
    NextcloudSettingsBase* m_settings = Q_NULLPTR;
    QNetworkReply* m_reply = Q_NULLPTR;

signals:
    void sslErrorOccured(QString md5Digest, QString sha1Digest);

};

#endif // WEBDAVCOMMANDENTITY_H
