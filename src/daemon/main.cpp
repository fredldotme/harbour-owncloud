#define DEBUG_WEBDAV 1

#include <QCoreApplication>
#include <QDBusConnection>
#include <QTimer>

#include <ownclouddbusconsts.h>

#include <webdavcommandqueue.h>
#include "filesystem.h"
#include "settings/nextcloudsettings.h"
#include "dbushandler.h"
#include "networkmonitor.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    NextcloudSettings::instance()->readSettings();

    // No need for the daemon to run in case "uploadAutomatically" is disabled
    if (!NextcloudSettings::instance()->uploadAutomatically()) {
        exit(0);
    }

    Filesystem *fsHandler = Filesystem::instance();
    DBusHandler *dbusHandler = new DBusHandler();
    NetworkMonitor *netMonitor = NetworkMonitor::instance();
    WebDavCommandQueue webDavQueue(&app, NextcloudSettings::instance());


    // Periodically check for existance of the local pictures path until found.
    // Don't stop the timer as the external storage could be ejected anytime.
    // Every 10 minutes should be enough in this specific case.
    QDir localPath(NextcloudSettings::instance()->localPicturesPath());
    QTimer localPathCheck;
    localPathCheck.setInterval(60000 * 10);
    localPathCheck.setSingleShot(false);
    QObject::connect(&localPathCheck, &QTimer::timeout, &localPathCheck,
                     [&localPath, &localPathCheck, fsHandler]() {
        if (localPath.exists())
            fsHandler->localPathChanged();
    });
    localPathCheck.start();

    //QObject::connect(fsHandler, &Filesystem::fileFound, &webDavQueue, &Uploader::fileFound);
    //QObject::connect(uploader, &Uploader::pokeFilesystemScanner, fsHandler, &Filesystem::localPathChanged);
    //QObject::connect(netMonitor, &NetworkMonitor::shouldDownloadChanged, uploader, &Uploader::setOnline);

    // DBus connections
    //QObject::connect(netMonitor, &NetworkMonitor::shouldDownloadChanged, dbusHandler, &DBusHandler::setOnline);
    //QObject::connect(uploader, &Uploader::fileUploaded, dbusHandler, &DBusHandler::fileUploaded);
    //QObject::connect(uploader, &Uploader::connectError, dbusHandler, &DBusHandler::connectError);
    //QObject::connect(uploader, &Uploader::uploadError, dbusHandler, &DBusHandler::uploadError);
    //QObject::connect(uploader, &Uploader::uploadingChanged, dbusHandler, &DBusHandler::uploadingChanged);
    //QObject::connect(dbusHandler, &DBusHandler::configChanged, uploader, &Uploader::settingsChanged);

    // We only need one instance
    if(!QDBusConnection::sessionBus().registerService(HarbourOwncloud::DBusConsts::DBUS_SERVICE) ||
            !QDBusConnection::sessionBus().registerObject(HarbourOwncloud::DBusConsts::DBUS_PATH, dbusHandler,
                                                          QDBusConnection::ExportAllSlots |
                                                          QDBusConnection::ExportAllProperties |
                                                          QDBusConnection::ExportAllSignals)) {
        exit(1);
    }

    netMonitor->recheckNetworks();
    //uploader->settingsChanged();

    return app.exec();
}
