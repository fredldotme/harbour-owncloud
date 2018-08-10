#ifndef CLOUDSTORAGEPROVIDER_H
#define CLOUDSTORAGEPROVIDER_H

#include <QObject>
#include <commandqueue.h>
#include <commandentity.h>
#include <settings/nextcloudsettingsbase.h>
#include <QDateTime>

class CloudStorageProvider : public CommandQueue
{
    Q_OBJECT

    Q_PROPERTY(NextcloudSettingsBase* settings READ settings WRITE setSettings NOTIFY settingsChanged)

public:
    explicit CloudStorageProvider(QObject *parent = Q_NULLPTR,
                                  NextcloudSettingsBase* settings = Q_NULLPTR);

    NextcloudSettingsBase* settings();
    void setSettings(NextcloudSettingsBase* v);

    virtual CommandEntity* fileDownloadRequest(QString from,
                                               QString mimeType = QStringLiteral(""),
                                               bool open = false,
                                               QDateTime lastModified = QDateTime()) = 0;
    virtual CommandEntity* fileUploadRequest(QString from,
                                             QString to,
                                             QDateTime lastModified = QDateTime()) = 0;
    virtual CommandEntity* makeDirectoryRequest(QString dirName) = 0;
    virtual CommandEntity* removeRequest(QString name) = 0;
    virtual CommandEntity* moveRequest(QString from, QString to) = 0;
    virtual CommandEntity* copyRequest(QString from, QString to) = 0;
    virtual CommandEntity* directoryListingRequest(QString path, bool refresh) = 0;

private:
    NextcloudSettingsBase* m_settings = Q_NULLPTR;

signals:
    void settingsChanged();

};

#endif // CLOUDSTORAGEPROVIDER_H
