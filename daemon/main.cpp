#define DEBUG_WEBDAV 1

#include <QCoreApplication>
#include <QDBusConnection>

#include <ownclouddbusconsts.h>

#include "filesystem.h"
#include "uploader.h"
#include "settings.h"
#include "dbushandler.h"
#include "networkmonitor.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Settings::instance()->readSettings();

    Filesystem *fsHandler = Filesystem::instance();
    Uploader *uploader = Uploader::instance();
    DBusHandler *dbusHandler = new DBusHandler();
    NetworkMonitor *netMonitor = NetworkMonitor::instance();

    QObject::connect(fsHandler, &Filesystem::fileFound, uploader, &Uploader::fileFound);
    QObject::connect(uploader, &Uploader::pokeFilesystemScanner, fsHandler, &Filesystem::localPathChanged);
    QObject::connect(netMonitor, &NetworkMonitor::shouldDownloadChanged, uploader, &Uploader::setOnline);

    // DBus connections
    QObject::connect(netMonitor, &NetworkMonitor::shouldDownloadChanged, dbusHandler, &DBusHandler::setOnline);
    QObject::connect(uploader, &Uploader::fileUploaded, dbusHandler, &DBusHandler::fileUploaded);
    QObject::connect(uploader, &Uploader::connectError, dbusHandler, &DBusHandler::connectError);
    QObject::connect(uploader, &Uploader::uploadError, dbusHandler, &DBusHandler::uploadError);
    QObject::connect(uploader, &Uploader::uploadingChanged, dbusHandler, &DBusHandler::uploadingChanged);
    QObject::connect(dbusHandler, &DBusHandler::configChanged, uploader, &Uploader::settingsChanged);

    // We only need one instance
    if(!QDBusConnection::sessionBus().registerService(HarbourOwncloud::DBusConsts::DBUS_SERVICE) ||
            !QDBusConnection::sessionBus().registerObject(HarbourOwncloud::DBusConsts::DBUS_PATH, dbusHandler,
                                                          QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllProperties)) {
        exit(1);
    }

    netMonitor->recheckNetworks();
    uploader->settingsChanged();

    return app.exec();
}
