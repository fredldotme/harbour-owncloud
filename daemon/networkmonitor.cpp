#include "networkmonitor.h"

NetworkMonitor::NetworkMonitor(QObject *parent) : QObject(parent)
{
    m_shouldDownload = false;
    m_uploadOverCellular = false;
    connect(&m_configManager, SIGNAL(configurationAdded(QNetworkConfiguration)), SLOT(recheckNetworks()));
    connect(&m_configManager, SIGNAL(configurationChanged(QNetworkConfiguration)), SLOT(recheckNetworks()));
    connect(&m_configManager, SIGNAL(configurationRemoved(QNetworkConfiguration)), SLOT(recheckNetworks()));
    connect(&m_configManager, SIGNAL(onlineStateChanged(bool)), SLOT(recheckNetworks()));
}

NetworkMonitor::~NetworkMonitor()
{

}

void NetworkMonitor::recheckNetworks()
{
    checkerMutex.lock();
    if (!m_configManager.isOnline()) {
        if (!m_shouldDownload) {
            checkerMutex.unlock();
            return;
        }

        m_shouldDownload = false;
        emit shouldDownloadChanged(false);
        checkerMutex.unlock();
        return;
    }

    // we are online, and we don't care what we upload over
    if (m_uploadOverCellular) {
        if (m_shouldDownload) {
            checkerMutex.unlock();
            return;
        }

        m_shouldDownload = true;
        emit shouldDownloadChanged(true);
        checkerMutex.unlock();
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
            checkerMutex.unlock();
            return;
        }

        m_shouldDownload = true;
        emit shouldDownloadChanged(true);
        checkerMutex.unlock();
        return;
    }

    if (!m_shouldDownload) {
        checkerMutex.unlock();
        return;
    }

    m_shouldDownload = false;
    emit shouldDownloadChanged(false);
    checkerMutex.unlock();
}

