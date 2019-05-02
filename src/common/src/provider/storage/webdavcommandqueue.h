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
                                AccountBase* settings = Q_NULLPTR);

public slots:
    virtual CommandEntity* fileDownloadRequest(const QString from,
                                               const QString mimeType = QStringLiteral(""),
                                               const bool open = false,
                                               const QDateTime lastModified = QDateTime(),
                                               const bool enqueue = true) Q_DECL_OVERRIDE;

    virtual CommandEntity* fileUploadRequest(const QString from,
                                             const QString to,
                                             const QDateTime lastModified = QDateTime(),
                                             const bool enqueue = true) Q_DECL_OVERRIDE;

    virtual CommandEntity* makeDirectoryRequest(const QString dirName,
                                                const bool enqueue = true) Q_DECL_OVERRIDE;

    virtual CommandEntity* removeRequest(const QString name,
                                         const bool enqueue = true) Q_DECL_OVERRIDE;

    virtual CommandEntity* moveRequest(const QString from,
                                       const QString to,
                                       const bool enqueue = true) Q_DECL_OVERRIDE;

    virtual CommandEntity* copyRequest(const QString from,
                                       const QString to,
                                       const bool enqueue = true) Q_DECL_OVERRIDE;

    virtual CommandEntity* directoryListingRequest(const QString path,
                                                   const bool refresh,
                                                   const bool enqueue = true) Q_DECL_OVERRIDE;

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
    CommandEntity* remoteLastModifiedRequest(const QString& destination,
                                            const QDateTime& lastModified);

    void updateConnectionSettings();

    QWebdav* m_client = Q_NULLPTR;

signals:
    void sslErrorOccured(QString md5Digest, QString sha1Digest);

};

#endif // WEBDAVCOMMANDQUEUE_H
