#ifndef WEBDAVCOMMANDENTITY_H
#define WEBDAVCOMMANDENTITY_H

#include <QObject>
#include "commandentity.h"
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
    void startWork();
    void abortWork();

    QWebdav* m_client = Q_NULLPTR;
    QNetworkReply* m_reply = Q_NULLPTR;

};

#endif // WEBDAVCOMMANDENTITY_H
