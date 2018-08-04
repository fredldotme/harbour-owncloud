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
    Q_PROPERTY(bool uploading READ uploading NOTIFY uploadingChanged)

    bool daemonInstalled();
    bool uploading();

public slots:
    void reloadConfig();
    void abort();

private:
    void daemonRegistered(const QString& serviceName);
    void daemonUnregistered(const QString& serviceName);
    bool isCurrentlyUploading();

    bool m_uploading;
    QDBusInterface* m_daemonInterface = Q_NULLPTR;
    QDBusServiceWatcher* m_daemonWatcher = Q_NULLPTR;

signals:
    void daemonInstalledChanged();
    void uploadingChanged();

private slots:
    void setUploading(bool value);
};

#endif // DAEMONCONTROL_H
