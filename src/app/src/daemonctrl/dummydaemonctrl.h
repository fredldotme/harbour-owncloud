#ifndef DAEMONCONTROL_H
#define DAEMONCONTROL_H

#include <QObject>

// Dummy implementation
class DaemonControl : public QObject
{
    Q_OBJECT
public:

    Q_PROPERTY(bool daemonInstalled READ daemonInstalled NOTIFY daemonInstalledChanged)
    Q_PROPERTY(bool uploading READ uploading NOTIFY uploadingChanged)

    explicit DaemonControl(QObject *parent = nullptr);

    bool uploading() { return false; }

public slots:
    bool daemonInstalled() { return false; }
    void reloadConfig() {}
    void abort() {}

signals:
    void daemonInstalledChanged();
    void uploadingChanged();

};

#endif // DAEMONCONTROL_H
