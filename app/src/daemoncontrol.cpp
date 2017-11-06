#include "daemoncontrol.h"

#include <ownclouddbusconsts.h>

DaemonControl::DaemonControl(QObject *parent) : QObject(parent)
{
    m_uploading = false;
    QDBusConnection::sessionBus().connect(HarbourOwncloud::DBusConsts::DBUS_SERVICE,
                                          HarbourOwncloud::DBusConsts::DBUS_PATH,
                                          HarbourOwncloud::DBusConsts::DBUS_INTERFACE,
                                          HarbourOwncloud::DBusConsts::DBUS_SIGNAL_UPLOADINGCHANGED,
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
    QDBusMessage message = QDBusMessage::createMethodCall(HarbourOwncloud::DBusConsts::DBUS_SERVICE,
                                                          HarbourOwncloud::DBusConsts::DBUS_PATH,
                                                          HarbourOwncloud::DBusConsts::DBUS_INTERFACE,
                                                          HarbourOwncloud::DBusConsts::DBUS_METHOD_RELOADCONFIG);
    QDBusConnection::sessionBus().send(message);
}
