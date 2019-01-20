#ifndef WEBDAVCOMMANDQUEUE_H
#define WEBDAVCOMMANDQUEUE_H

#include <QObject>
#include "cloudstorageprovider.h"
#include "commandqueue.h"

#include <settings/nextcloudsettingsbase.h>
#include <qwebdav.h>


class WebDavCommandQueue : public CloudStorageProvider
{
    Q_OBJECT

public:
    explicit WebDavCommandQueue(QObject* parent = Q_NULLPTR,
                                NextcloudSettingsBase* settings = Q_NULLPTR);

public slots:
    virtual CommandEntity* fileDownloadRequest(QString from,
                                               QString mimeType = QStringLiteral(""),
                                               bool open = false,
                                               QDateTime lastModified = QDateTime()) Q_DECL_OVERRIDE;
    virtual CommandEntity* fileUploadRequest(QString from,
                                             QString to,
                                             QDateTime lastModified = QDateTime()) Q_DECL_OVERRIDE;
    virtual CommandEntity* makeDirectoryRequest(QString dirName) Q_DECL_OVERRIDE;
    virtual CommandEntity* removeRequest(QString name) Q_DECL_OVERRIDE;
    virtual CommandEntity* moveRequest(QString from, QString to) Q_DECL_OVERRIDE;
    virtual CommandEntity* copyRequest(QString from, QString to) Q_DECL_OVERRIDE;
    virtual CommandEntity* directoryListingRequest(QString path, bool refresh) Q_DECL_OVERRIDE;

    virtual bool supportsQFile() Q_DECL_OVERRIDE {
        return false;
    }

    QWebdav* getWebdav()
    {
        qDebug() << Q_FUNC_INFO << m_client;
        return m_client;
    }

private:
    CommandEntity* localLastModifiedRequest(const QString& destination,
                                            const QDateTime& lastModified);
    CommandEntity* openFileRequest(const QString& destination);
    CommandEntity* remoteLastModifiedRequest(const QString& destination,
                                            const QDateTime& lastModified);

    void updateConnectionSettings();

    QWebdav* m_client = Q_NULLPTR;

signals:
    void sslErrorOccured(QString md5Digest, QString sha1Digest);

};

#endif // WEBDAVCOMMANDQUEUE_H
