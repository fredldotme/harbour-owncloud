#include "daemoncontrol.h"

DaemonControl::DaemonControl(QObject *parent) :
    QObject(parent)
{
}

bool DaemonControl::daemonInstalled()
{
    return QFile("/usr/bin/harbour-owncloud-daemon").exists();
}

void DaemonControl::reloadConfig()
{
    QDBusMessage message = QDBusMessage::createMethodCall("com.github.beidl.HarbourOwncloud.Daemon",
                                                          "/",
                                                          "com.github.beidl.HarbourOwncloud.Daemon.Uploader",
                                                          "reloadConfig");
    QDBusConnection::sessionBus().send(message);
}
