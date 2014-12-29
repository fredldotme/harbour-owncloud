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

    Q_INVOKABLE void reloadConfig();

private:
    bool m_uploading;

    bool daemonInstalled();
    bool uploading();

signals:
    void daemonInstalledChanged();
    void uploadingChanged();

private slots:
    void setUploading(bool value);
};

#endif // DAEMONCONTROL_H
