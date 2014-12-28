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

private:
    bool m_daemonRunning;

    bool daemonInstalled();

signals:
    void daemonInstalledChanged();

public slots:
    void reloadConfig();

};

#endif // DAEMONCONTROL_H
