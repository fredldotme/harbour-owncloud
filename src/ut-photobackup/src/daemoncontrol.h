#ifndef DAEMONCONTROL_H
#define DAEMONCONTROL_H

#include <QObject>
#include <QFile>

#ifndef QT_NO_DBUS
#include <QtDBus>
#endif

class DaemonControl : public QObject
{
    Q_OBJECT
public:
    explicit DaemonControl(QObject *parent = Q_NULLPTR);

    Q_PROPERTY(bool daemonInstalled READ daemonInstalled NOTIFY daemonInstalledChanged)
    Q_PROPERTY(bool uploading READ uploading NOTIFY uploadingChanged)

    bool uploading();

public slots:
    void reloadConfig();
    void abort();
    bool daemonInstalled();

private:
    void daemonRegistered(const QString& serviceName);
    void daemonUnregistered(const QString& serviceName);
    bool isCurrentlyUploading();

    bool m_uploading;

#ifndef QT_NO_DBUS
    QDBusInterface* m_daemonInterface = Q_NULLPTR;
    QDBusServiceWatcher* m_daemonWatcher = Q_NULLPTR;
#endif

signals:
    void daemonInstalledChanged();
    void uploadingChanged();

private slots:
    void setUploading(bool value);
};

#endif // DAEMONCONTROL_H
