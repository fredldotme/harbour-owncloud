#include "daemoncontrol.h"

#include <ownclouddbusconsts.h>

DaemonControl::DaemonControl(QObject *parent) : QObject(parent)
{
    this->m_uploading = false;

    QDBusServiceWatcher::WatchMode daemonWatchFlags =
            (QDBusServiceWatcher::WatchForRegistration |
             QDBusServiceWatcher::WatchForUnregistration);

    this->m_daemonWatcher =
            new QDBusServiceWatcher(HarbourOwncloud::DBusConsts::DBUS_SERVICE,
                                    QDBusConnection::sessionBus(),
                                    daemonWatchFlags,
                                    this);

    QObject::connect(this->m_daemonWatcher, &QDBusServiceWatcher::serviceRegistered,
                     this, &DaemonControl::daemonRegistered);
    QObject::connect(this->m_daemonWatcher, &QDBusServiceWatcher::serviceUnregistered,
                     this, &DaemonControl::daemonUnregistered);


    this->m_daemonInterface =
            new QDBusInterface(HarbourOwncloud::DBusConsts::DBUS_SERVICE,
                               HarbourOwncloud::DBusConsts::DBUS_PATH,
                               HarbourOwncloud::DBusConsts::DBUS_INTERFACE,
                               QDBusConnection::sessionBus(),
                               this);
    this->m_daemonInterface->setTimeout(1000);

    setUploading(isCurrentlyUploading());

    QDBusConnection::sessionBus()
            .connect(HarbourOwncloud::DBusConsts::DBUS_SERVICE,
                     HarbourOwncloud::DBusConsts::DBUS_PATH,
                     HarbourOwncloud::DBusConsts::DBUS_INTERFACE,
                     HarbourOwncloud::DBusConsts::DBUS_SIGNAL_UPLOADINGCHANGED,
                     this,
                     SLOT(setUploading(bool)));
}

bool DaemonControl::daemonInstalled()
{
    const QString BINARY_NAME = QStringLiteral("harbour-owncloud-daemon");
    const QString ABSOLUTE_PATH = qApp->applicationDirPath() + "/" + BINARY_NAME;
    return QFile(ABSOLUTE_PATH).exists();
}

bool DaemonControl::uploading()
{
    return this->m_uploading;
}

void DaemonControl::setUploading(bool value)
{
    qDebug() << "Uploading? " << value;
    if(value != this->m_uploading) {
        this->m_uploading = value;
        emit uploadingChanged();
    }
}

void DaemonControl::reloadConfig()
{
    if (!daemonInstalled())
        return;

    this->m_daemonInterface->
            asyncCall(HarbourOwncloud::DBusConsts::DBUS_METHOD_RELOADCONFIG);
}

void DaemonControl::abort()
{
    if (!daemonInstalled())
        return;

    this->m_daemonInterface->
            asyncCall(HarbourOwncloud::DBusConsts::DBUS_METHOD_ABORT);
}

void DaemonControl::daemonRegistered(const QString &serviceName)
{
    qDebug() << serviceName;

    setUploading(isCurrentlyUploading());
}

void DaemonControl::daemonUnregistered(const QString &serviceName)
{
    qDebug() << serviceName;
    setUploading(false);
}

bool DaemonControl::isCurrentlyUploading()
{
    const QVariant currentlyUploading =
            this->m_daemonInterface
            ->property(HarbourOwncloud::DBusConsts::DBUS_PROPERTY_UPLOADING.toUtf8().data());
    return (currentlyUploading.isValid() && currentlyUploading.toBool());
}
