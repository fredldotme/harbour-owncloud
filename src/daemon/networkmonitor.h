#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QObject>
#include <QNetworkConfigurationManager>
#include <QMutex>
#include <QMutexLocker>

#include <settings/nextcloudsettings.h>

class NetworkMonitor : public QObject
{
    Q_OBJECT
public:
    explicit NetworkMonitor(QObject *parent = Q_NULLPTR,
                            NextcloudSettings* settings = Q_NULLPTR);
    ~NetworkMonitor();
    static NetworkMonitor* instance(NextcloudSettings* settings);
    bool shouldDownload() { return m_shouldDownload; }

signals:
    void shouldDownloadChanged(bool);

public slots:
    void recheckNetworks();

private:
    QNetworkConfigurationManager m_configManager;
    NextcloudSettings* m_settings = Q_NULLPTR;
    bool m_shouldDownload;
    QMutex checkerMutex;
};

#endif // NETWORKMONITOR_H
