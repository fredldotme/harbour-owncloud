#ifndef UTFILEUPLOADCOMMANDENTITY_H
#define UTFILEUPLOADCOMMANDENTITY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include "commandentity.h"
#include <settings/nextcloudsettingsbase.h>
#include <lomiri/download_manager/upload.h>
#include <lomiri/download_manager/manager.h>

class UtFileUploadCommandEntity : public CommandEntity
{
    Q_OBJECT

public:
    explicit UtFileUploadCommandEntity(QObject* parent = Q_NULLPTR,
                                       QString localPath = QStringLiteral(""),
                                       QString remotePath = QStringLiteral(""),
                                       AccountBase* settings = Q_NULLPTR);

protected:
    bool startWork() Q_DECL_OVERRIDE;
    bool staticProgress() const Q_DECL_OVERRIDE { return false; }

private:
    Lomiri::DownloadManager::Manager* m_dlManager = Q_NULLPTR;
    Lomiri::DownloadManager::Download* m_download = Q_NULLPTR;
    AccountBase* m_settings = Q_NULLPTR;
    bool m_running = false;
    QFile* m_localFile = Q_NULLPTR;
    QString m_remotePath;
};

#endif // UTFILEUPLOADCOMMANDENTITY_H
