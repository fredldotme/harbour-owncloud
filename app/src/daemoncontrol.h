#ifndef DAEMONCONTROL_H
#define DAEMONCONTROL_H

#include <QObject>
#include <QFile>
#include <QtDBus>

class DaemonControl : public QObject
{
    Q_OBJECT
public:
    explicit DaemonControl(QObject *parent = 0);

    Q_PROPERTY(bool daemonInstalled READ daemonInstalled NOTIFY daemonInstalledChanged)
    Q_PROPERTY(bool daemonRunning READ daemonRunning WRITE setDaemonRunning NOTIFY daemonRunningChanged)

private:
    bool m_daemonRunning;

    bool daemonInstalled();
    bool daemonRunning();
    void setDaemonRunning(bool value);

signals:
    void daemonInstalledChanged();
    void daemonRunningChanged();

public slots:
    void reloadConfig();

};

#endif // DAEMONCONTROL_H
