#include "networkmonitor.h"

#include <QCoreApplication>

NetworkMonitor::NetworkMonitor(QObject *parent,
                               NextcloudSettingsBase* settings) :
    QObject(parent), m_settings(settings)
{
    this->m_shouldSync = false;
    connect(&this->m_configManager, &QNetworkConfigurationManager::configurationAdded,
            this, &NetworkMonitor::recheckNetworks);
    connect(&this->m_configManager, &QNetworkConfigurationManager::configurationChanged,
            this, &NetworkMonitor::recheckNetworks);
    connect(&this->m_configManager, &QNetworkConfigurationManager::configurationRemoved,
            this, &NetworkMonitor::recheckNetworks);
    connect(&this->m_configManager, &QNetworkConfigurationManager::onlineStateChanged,
            this, &NetworkMonitor::recheckNetworks);
}

NetworkMonitor::~NetworkMonitor()
{

}

void NetworkMonitor::recheckNetworks()
{
    QMutexLocker locker(&checkerMutex);

    if (this->m_settings) {
        const bool uploadOverCellular = this->m_settings->mobileUpload();
        const bool uploadAutomatically = this->m_settings->uploadAutomatically();

        if (!uploadAutomatically) {
            setShouldDownload(false);
            return;
        }

        // Don't explicitly check for bearer types in case cellular
        // upload is enabled, fall through otherwise
        if (uploadOverCellular) {
            setShouldDownload(true);
            return;
        }
    }

    if (!m_configManager.isOnline()) {
        setShouldDownload(false);
        return;
    }

    bool hasNonCellular = false;
    foreach (const QNetworkConfiguration &config, m_configManager.allConfigurations()) {
        if ((config.bearerType() == QNetworkConfiguration::BearerWLAN ||
            config.bearerType() == QNetworkConfiguration::BearerEthernet) &&
                config.state() == QNetworkConfiguration::Active) {
            hasNonCellular = true;
            break;
        }
    }

    if (hasNonCellular) {
        setShouldDownload(true);
        return;
    }

    setShouldDownload(false);
}

void NetworkMonitor::setShouldDownload(bool value)
{
    if (this->m_shouldSync == value) {
        return;
    }

    this->m_shouldSync = value;
    Q_EMIT shouldSyncChanged(value);
}
