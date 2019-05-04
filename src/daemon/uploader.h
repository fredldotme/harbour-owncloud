#ifndef UPLOADER_H
#define UPLOADER_H

#include <QObject>
#include <provider/storage/webdavcommandqueue.h>
#include <settings/nextcloudsettingsbase.h>
#include "networkmonitor.h"

class Uploader : public QObject
{
    Q_OBJECT
public:
    explicit Uploader(QObject *parent = Q_NULLPTR,
                      const QString& targetDirectory = QStringLiteral(""),
                      NetworkMonitor* networkMonitor = Q_NULLPTR,
                      AccountBase* settings = Q_NULLPTR);
public:
    bool running();
    bool shouldSync();

public slots:
    void triggerSync(const QString &localPath, const QString &remoteSubdir);
    void stopSync();

private:
    const QString m_targetDirectory;
    NetworkMonitor* m_networkMonitor = Q_NULLPTR;
    AccountBase* m_settings = Q_NULLPTR;
    WebDavCommandQueue* m_webDavCommandQueue = Q_NULLPTR;
    QSet<QString> m_syncingPaths;

signals:
    void runningChanged();

};

#endif // UPLOADER_H
