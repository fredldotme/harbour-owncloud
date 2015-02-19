#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QObject>
#include <QNetworkConfigurationManager>
#include <QMutex>

class NetworkMonitor : public QObject
{
    Q_OBJECT
public:
    explicit NetworkMonitor(QObject *parent = 0);
    ~NetworkMonitor();

    void setUploadOverCellullar(bool enabled);

    bool shouldDownload() { return m_shouldDownload; }

signals:
    void shouldDownloadChanged(bool);

public slots:
    void recheckNetworks();

private:
    QNetworkConfigurationManager m_configManager;
    bool m_shouldDownload;
    bool m_uploadOverCellular;
    QMutex checkerMutex;
};

#endif // NETWORKMONITOR_H
