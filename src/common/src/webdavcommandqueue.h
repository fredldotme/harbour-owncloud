#ifndef WEBDAVCOMMANDQUEUE_H
#define WEBDAVCOMMANDQUEUE_H

#include <QObject>
#include "commandqueue.h"

#include <settings/nextcloudsettingsbase.h>
#include <qwebdav.h>


class WebDavCommandQueue : public CommandQueue
{
    Q_OBJECT

    Q_PROPERTY(NextcloudSettingsBase* settings READ settings WRITE setSettings NOTIFY settingsChanged)

public:
    explicit WebDavCommandQueue(QObject* parent = Q_NULLPTR,
                                NextcloudSettingsBase* settings = Q_NULLPTR);

    NextcloudSettingsBase* settings();
    void setSettings(NextcloudSettingsBase* v);

public slots:
    CommandEntity* fileDownloadRequest(QString from,
                                       QString mimeType = QStringLiteral(""),
                                       bool open = false,
                                       QDateTime lastModified = QDateTime());
    CommandEntity* fileUploadRequest(QString from,
                                     QString to,
                                     QDateTime lastModified = QDateTime());
    CommandEntity* makeDirectoryRequest(QString dirName);
    CommandEntity* removeRequest(QString name);
    CommandEntity* moveRequest(QString from, QString to);
    CommandEntity* copyRequest(QString from, QString to);
    CommandEntity* directoryListingRequest(QString path);

protected:
    QWebdav* getWebdav() { return m_client; }

private:
    NextcloudSettingsBase* m_settings = Q_NULLPTR;
    QWebdav* m_client = Q_NULLPTR;

signals:
    void settingsChanged();
    void sslErrorOccured(QString md5Digest, QString sha1Digest);

};

#endif // WEBDAVCOMMANDQUEUE_H
