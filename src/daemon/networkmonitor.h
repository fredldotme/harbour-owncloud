#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QObject>
#include <QNetworkConfigurationManager>
#include <QMutex>
#include <QMutexLocker>

#include <settings/inifilesettings.h>

class NetworkMonitor : public QObject
{
    Q_OBJECT
public:
    explicit NetworkMonitor(QObject *parent = Q_NULLPTR,
                            IniFileSettings* settings = Q_NULLPTR);
    ~NetworkMonitor();
    static NetworkMonitor* instance(IniFileSettings* settings);
    bool shouldSync() { return m_shouldSync; }

signals:
    void shouldSyncChanged(bool);

public slots:
    void recheckNetworks();

private:
    void setShouldDownload(bool value);

    QNetworkConfigurationManager m_configManager;
    IniFileSettings* m_settings = Q_NULLPTR;
    bool m_shouldSync;
    QMutex checkerMutex;
};

#endif // NETWORKMONITOR_H
