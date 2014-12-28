#include "daemoncontrol.h"

DaemonControl::DaemonControl(QObject *parent) :
    QObject(parent)
{
}

bool DaemonControl::daemonInstalled()
{
    return QFile("/usr/bin/harbour-owncloud-daemon").exists();
}

bool DaemonControl::daemonRunning()
{
    if(QDBusConnection::sessionBus().registerService("com.github.beidl.harbour-owncloud.daemon")) {
        QDBusConnection::sessionBus().unregisterService("com.github.beidl.harbour-owncloud.daemon");
        return false;
    } else {
        return true;
    }
}

void DaemonControl::setDaemonRunning(bool value)
{
    QProcess *proc = new QProcess();
    proc->setReadChannel(QProcess::StandardOutput);
    connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
    QStringList args;
    args << "--user";
    if(value) {
        args << "start";
    } else {
        args << "stop";
    }
    args << "harbour-owncloud-daemon.service";
    proc->start("systemctl", args);
}

void DaemonControl::reloadConfig()
{
    QDBusMessage message = QDBusMessage::createMethodCall("com.github.beidl.harbour-owncloud.daemon",
                                                          "/HarbourOwncloudDaemon",
                                                          "",
                                                          "reloadConfig");
    QDBusConnection::sessionBus().send(message);
}
