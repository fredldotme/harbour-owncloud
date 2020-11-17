#ifndef UTFILEDOWNLOADCOMMANDENTITY_H
#define UTFILEDOWNLOADCOMMANDENTITY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include "commandentity.h"
#include <settings/nextcloudsettingsbase.h>
#include <ubuntu/download_manager/download.h>
#include <ubuntu/download_manager/manager.h>

class UtFileDownloadCommandEntity : public CommandEntity
{
    Q_OBJECT

public:
    explicit UtFileDownloadCommandEntity(QObject* parent = Q_NULLPTR,
                                       QString remotePath = QStringLiteral(""),
                                       QString localPath = QStringLiteral(""),
                                       AccountBase* settings = Q_NULLPTR);
    ~UtFileDownloadCommandEntity();

protected:
    bool startWork() Q_DECL_OVERRIDE;
    bool abortWork() Q_DECL_OVERRIDE;
    bool staticProgress() const Q_DECL_OVERRIDE { return false; }

    QString m_remotePath = QStringLiteral("");
    QString m_localPath = QStringLiteral("");
    QDir m_localDir;

private Q_SLOTS:
    void handleProgress(qulonglong received, qulonglong total);
    void handleFinished(const QString& path);
    void handleError(Ubuntu::DownloadManager::Error* error);

private:
    Ubuntu::DownloadManager::Manager* m_dlManager = Q_NULLPTR;
    Ubuntu::DownloadManager::Download* m_download = Q_NULLPTR;
    AccountBase* m_settings = Q_NULLPTR;
    bool m_running = false;
};

#endif // UTFILEDOWNLOADCOMMANDENTITY_H
