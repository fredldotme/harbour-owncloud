#ifndef CLOUDSTORAGEPROVIDER_H
#define CLOUDSTORAGEPROVIDER_H

#include <QObject>
#include <provider/settingsbackedcommandqueue.h>
#include <commandentity.h>
#include <settings/nextcloudsettingsbase.h>
#include <QDateTime>

class CloudStorageProvider : public SettingsBackedCommandQueue
{
    Q_OBJECT

public:
    explicit CloudStorageProvider(QObject *parent = Q_NULLPTR,
                                  AccountBase* settings = Q_NULLPTR);

    virtual CommandEntity* fileDownloadRequest(const QString from,
                                               const QString mimeType = QStringLiteral(""),
                                               const bool open = false,
                                               const QDateTime lastModified = QDateTime(),
                                               const bool enqueue = false,
                                               const QString customDestination = QStringLiteral(""))
    {
        Q_UNUSED(from);
        Q_UNUSED(mimeType);
        Q_UNUSED(open);
        Q_UNUSED(lastModified);
        Q_UNUSED(enqueue);
        Q_UNUSED(customDestination);
        return Q_NULLPTR;
    }

    virtual CommandEntity* fileUploadRequest(const QString from,
                                             const QString to,
                                             const QDateTime lastModified = QDateTime(),
                                             const bool enqueue = false)
    {
        Q_UNUSED(from);
        Q_UNUSED(to);
        Q_UNUSED(lastModified);
        Q_UNUSED(enqueue);
        return Q_NULLPTR;
    }

    virtual CommandEntity* makeDirectoryRequest(const QString dirName,
                                                const bool enqueue = false)
    {
        Q_UNUSED(dirName);
        Q_UNUSED(enqueue);
        return Q_NULLPTR;
    }

    virtual CommandEntity* removeRequest(const QString name,
                                         const bool enqueue = false)
    {
        Q_UNUSED(name);
        Q_UNUSED(enqueue);
        return Q_NULLPTR;
    }

    virtual CommandEntity* moveRequest(const QString from,
                                       const QString to,
                                       const bool enqueue = false)
    {
        Q_UNUSED(from);
        Q_UNUSED(to);
        Q_UNUSED(enqueue);
        return Q_NULLPTR;
    }

    virtual CommandEntity* copyRequest(const QString from,
                                       const QString to,
                                       const bool enqueue = false)
    {
        Q_UNUSED(from);
        Q_UNUSED(to);
        Q_UNUSED(enqueue);
        return Q_NULLPTR;
    }

    virtual CommandEntity* directoryListingRequest(const QString path,
                                                   const bool refresh,
                                                   const bool enqueue = false)
    {
        Q_UNUSED(path);
        Q_UNUSED(refresh);
        Q_UNUSED(enqueue);
        return Q_NULLPTR;
    }

    virtual bool supportsQFile()
    {
        return false;
    }
};

#endif // CLOUDSTORAGEPROVIDER_H
