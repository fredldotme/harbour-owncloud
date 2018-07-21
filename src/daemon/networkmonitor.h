#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QObject>
#include <QNetworkConfigurationManager>
#include <QMutex>
#include <QMutexLocker>

#include "settings/nextcloudsettings.h"

class NetworkMonitor : public QObject
{
    Q_OBJECT
public:
    explicit NetworkMonitor(QObject *parent = 0);
    ~NetworkMonitor();
    static NetworkMonitor* instance();

    void setUploadOverCellullar(bool enabled);

    bool shouldDownload() { return NextcloudSettings::instance()->mobileUpload() || m_shouldDownload; }

signals:
    void shouldDownloadChanged(bool);

public slots:
    void recheckNetworks();

private:
    QNetworkConfigurationManager m_configManager;
    bool m_shouldDownload;
    QMutex checkerMutex;
};

#endif // NETWORKMONITOR_H
