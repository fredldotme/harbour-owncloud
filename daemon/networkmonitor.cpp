#include "networkmonitor.h"

NetworkMonitor::NetworkMonitor(QObject *parent) : QObject(parent)
{
    m_shouldDownload = false;
    connect(&m_configManager, SIGNAL(configurationAdded(QNetworkConfiguration)), SLOT(recheckNetworks()));
    connect(&m_configManager, SIGNAL(configurationChanged(QNetworkConfiguration)), SLOT(recheckNetworks()));
    connect(&m_configManager, SIGNAL(configurationRemoved(QNetworkConfiguration)), SLOT(recheckNetworks()));
    connect(&m_configManager, SIGNAL(onlineStateChanged(bool)), SLOT(recheckNetworks()));
}

NetworkMonitor::~NetworkMonitor()
{

}

NetworkMonitor* NetworkMonitor::instance()
{
    static NetworkMonitor monitor;
    return &monitor;
}

void NetworkMonitor::recheckNetworks()
{
    QMutexLocker locker(&checkerMutex);

    bool uploadOverCellular = Settings::instance()->mobileUpload();

    if (!m_configManager.isOnline()) {
        if (!m_shouldDownload) {
            return;
        }

        m_shouldDownload = false;
        emit shouldDownloadChanged(false);
        return;
    }

    // we are online, and we don't care what we upload over
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
        if (config.bearerType() == QNetworkConfiguration::BearerWLAN ||
            config.bearerType() == QNetworkConfiguration::BearerEthernet) {
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

