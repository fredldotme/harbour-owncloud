#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QObject>
#include <QNetworkConfigurationManager>
#include <QMutex>
#include <QMutexLocker>

#include <settings/nextcloudsettingsbase.h>

class NetworkMonitor : public QObject
{
    Q_OBJECT
public:
    explicit NetworkMonitor(QObject *parent = Q_NULLPTR,
                            AccountBase* settings = Q_NULLPTR);
    ~NetworkMonitor();
    bool shouldSync() { return m_shouldSync; }

signals:
    void shouldSyncChanged(bool);

public slots:
    void recheckNetworks();

private:
    void setShouldDownload(bool value);

    QNetworkConfigurationManager m_configManager;
    AccountBase* m_settings = Q_NULLPTR;
    bool m_shouldSync;
    QMutex checkerMutex;
};

#endif // NETWORKMONITOR_H
