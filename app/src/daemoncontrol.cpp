#include "daemoncontrol.h"

DaemonControl::DaemonControl(QObject *parent) : QObject(parent)
{
    m_uploading = false;
    QDBusConnection::sessionBus().connect("com.github.beidl.HarbourOwncloud.Daemon",
                                          "/",
                                          "com.github.beidl.HarbourOwncloud.Daemon.Uploader",
                                          "uploadingChanged",
                                          this,
                                          SLOT(setUploading(bool)));
}

bool DaemonControl::daemonInstalled()
{
    return QFile("/usr/bin/harbour-owncloud-daemon").exists();
}

bool DaemonControl::uploading()
{
    return m_uploading;
}

void DaemonControl::setUploading(bool value)
{
    qDebug() << "Uploading? " << value;
    if(value != m_uploading) {
        m_uploading = value;
        emit uploadingChanged();
    }
}

void DaemonControl::reloadConfig()
{
    QDBusMessage message = QDBusMessage::createMethodCall("com.github.beidl.HarbourOwncloud.Daemon",
                                                          "/",
                                                          "com.github.beidl.HarbourOwncloud.Daemon.Uploader",
                                                          "reloadConfig");
    QDBusConnection::sessionBus().send(message);
}
