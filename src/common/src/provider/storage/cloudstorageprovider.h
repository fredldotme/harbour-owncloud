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

    virtual CommandEntity* fileDownloadRequest(QString from,
                                               QString mimeType = QStringLiteral(""),
                                               bool open = false,
                                               QDateTime lastModified = QDateTime())
    {
        Q_UNUSED(from);
        Q_UNUSED(mimeType);
        Q_UNUSED(open);
        Q_UNUSED(lastModified);
        return Q_NULLPTR;
    }

    virtual CommandEntity* fileUploadRequest(QString from,
                                             QString to,
                                             QDateTime lastModified = QDateTime())
    {

        Q_UNUSED(from);
        Q_UNUSED(to);
        Q_UNUSED(lastModified);
        return Q_NULLPTR;
    }

    virtual CommandEntity* makeDirectoryRequest(QString dirName)
    {

        Q_UNUSED(dirName);
        return Q_NULLPTR;
    }

    virtual CommandEntity* removeRequest(QString name)
    {

        Q_UNUSED(name);
        return Q_NULLPTR;
    }

    virtual CommandEntity* moveRequest(QString from, QString to)
    {
        Q_UNUSED(from);
        Q_UNUSED(to);
        return Q_NULLPTR;
    }

    virtual CommandEntity* copyRequest(QString from, QString to)
    {
        Q_UNUSED(from);
        Q_UNUSED(to);
        return Q_NULLPTR;
    }

    virtual CommandEntity* directoryListingRequest(QString path, bool refresh)
    {
        Q_UNUSED(path);
        Q_UNUSED(refresh);
        return Q_NULLPTR;
    }

    virtual bool supportsQFile()
    {
        return false;
    }
};

#endif // CLOUDSTORAGEPROVIDER_H
