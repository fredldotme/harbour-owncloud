#include "networkmonitor.h"

#include <settings/nextcloudsettings.h>
#include <QCoreApplication>

NetworkMonitor::NetworkMonitor(QObject *parent,
                               NextcloudSettings* settings) :
    QObject(parent), m_settings(settings)
{
    m_shouldDownload = false;
    connect(&m_configManager, &QNetworkConfigurationManager::configurationAdded, this, &NetworkMonitor::recheckNetworks);
    connect(&m_configManager, &QNetworkConfigurationManager::configurationChanged, this, &NetworkMonitor::recheckNetworks);
    connect(&m_configManager, &QNetworkConfigurationManager::configurationRemoved, this, &NetworkMonitor::recheckNetworks);
    connect(&m_configManager, &QNetworkConfigurationManager::onlineStateChanged, this, &NetworkMonitor::recheckNetworks);
}

NetworkMonitor::~NetworkMonitor()
{

}

NetworkMonitor* NetworkMonitor::instance(NextcloudSettings* settings)
{
    static NetworkMonitor monitor(qApp, settings);
    return &monitor;
}

void NetworkMonitor::recheckNetworks()
{
    QMutexLocker locker(&checkerMutex);

    const bool uploadOverCellular = this->m_settings->mobileUpload();

    if (!m_configManager.isOnline()) {
        if (!m_shouldDownload) {
            return;
        }

        m_shouldDownload = false;
        emit shouldDownloadChanged(false);
        return;
    }

    // Don't explicitly check for bearer types in case cellular
    // upload is enabled, fall through otherwise
    if (uploadOverCellular) {
        if (m_shouldDownload) {
            return;
        }

        m_shouldDownload = true;
        emit shouldDownloadChanged(true);
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
        if (m_shouldDownload) {
            return;
        }

        m_shouldDownload = true;
        emit shouldDownloadChanged(true);
        return;
    }

    if (!m_shouldDownload) {
        return;
    }

    m_shouldDownload = false;
    emit shouldDownloadChanged(false);
}
