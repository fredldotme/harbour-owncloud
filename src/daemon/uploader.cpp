#include "uploader.h"

#include <commands/sync/ncsynccommandunit.h>

Uploader::Uploader(QObject *parent,
                   const QString& targetDirectory,
                   NetworkMonitor* networkMonitor,
                   NextcloudSettingsBase* settings) :
    QObject(parent),
    m_targetDirectory(targetDirectory),
    m_networkMonitor(networkMonitor),
    m_settings(settings),
    m_webDavCommandQueue(new WebDavCommandQueue(this, settings))
{
    this->m_webDavCommandQueue->setImmediate(true);
    QObject::connect(this->m_webDavCommandQueue, &WebDavCommandQueue::runningChanged,
                     this, &Uploader::runningChanged);
}

void Uploader::triggerSync()
{
    if (!(this->m_webDavCommandQueue && this->m_settings && this->m_networkMonitor)) {
        qCritical() << "Invalid object existance (webDavQueue, settings, netMonitor), "
                    << "bailing out.";
        return;
    }

    qDebug() << "shouldDownload" << this->m_networkMonitor->shouldSync();
    qDebug() << "uploadAutomatically" << this->m_settings->uploadAutomatically();

    if (!(shouldSync())) {
        this->m_webDavCommandQueue->stop();
        return;
    }

    if (this->m_webDavCommandQueue->queue().length() > 0)
        return;

    NcSyncCommandUnit* syncDirectoriesUnit =
            new NcSyncCommandUnit(this->m_webDavCommandQueue,
                                  this->m_webDavCommandQueue->getWebdav(),
                                  this->m_settings->localPicturesPath(),
                                  this->m_targetDirectory);

    this->m_webDavCommandQueue->enqueue((CommandEntity*)syncDirectoriesUnit);
}

void Uploader::stopSync()
{
    if (!this->m_webDavCommandQueue) {
        qCritical() << "Invalid object existance (webDavQueue, settings, netMonitor), "
                    << "bailing out.";
        return;
    }
    this->m_webDavCommandQueue->stop();
}

bool Uploader::running()
{
    if (!this->m_webDavCommandQueue) {
        qCritical() << "Invalid object existance (webDavQueue), "
                    << "bailing out.";
        return false;
    }
    return this->m_webDavCommandQueue->isRunning();
}

bool Uploader::shouldSync()
{
    if (!(this->m_settings && this->m_networkMonitor)) {
        qCritical() << "Invalid object existance (settings, netMonitor), "
                    << "bailing out.";
        return false;
    }

    return (this->m_networkMonitor->shouldSync() && this->m_settings->uploadAutomatically());
}
