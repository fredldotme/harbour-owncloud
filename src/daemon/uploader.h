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
                      const QString& localPath = QStringLiteral(""),
                      const QString& targetDirectory = QStringLiteral(""),
                      NetworkMonitor* networkMonitor = Q_NULLPTR,
                      AccountBase* settings = Q_NULLPTR);
public:
    bool running();
    bool shouldSync();

public slots:
    void triggerSync();
    void stopSync();

private:
    const QString m_localPath;
    const QString m_targetDirectory;
    NetworkMonitor* m_networkMonitor = Q_NULLPTR;
    AccountBase* m_settings = Q_NULLPTR;
    WebDavCommandQueue* m_webDavCommandQueue = Q_NULLPTR;

signals:
    void runningChanged();

};

#endif // UPLOADER_H
