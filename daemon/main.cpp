#define DEBUG_WEBDAV 1
#include "filesystem.h"
#include "uploader.h"
#include "settings.h"
#include "dbushandler.h"
#include "networkmonitor.h"

#include <QCoreApplication>
#include <QDBusConnection>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Settings::instance()->readSettings();

    Filesystem *fsHandler = Filesystem::instance();
    Uploader *uploader = Uploader::instance();
    DBusHandler *dbusHandler = new DBusHandler(uploader);
    NetworkMonitor *netMonitor = NetworkMonitor::instance();

    QObject::connect(fsHandler, SIGNAL(fileFound(QString)), uploader, SLOT(fileFound(QString)));
    QObject::connect(uploader, SIGNAL(pokeFilesystemScanner()), fsHandler, SLOT(localPathChanged()));
    QObject::connect(netMonitor, SIGNAL(shouldDownloadChanged(bool)), uploader, SLOT(setOnline(bool)));

    // DBus connections
    QObject::connect(netMonitor, SIGNAL(shouldDownloadChanged(bool)), dbusHandler, SLOT(setOnline(bool)));
    QObject::connect(uploader, SIGNAL(fileUploaded(QString)), dbusHandler, SIGNAL(fileUploaded(QString)));
    QObject::connect(uploader, SIGNAL(connectError(QString)), dbusHandler, SIGNAL(connectError(QString)));
    QObject::connect(uploader, SIGNAL(uploadError(QString)), dbusHandler, SIGNAL(uploadError(QString)));
    QObject::connect(uploader, SIGNAL(uploadingChanged(bool)), dbusHandler, SIGNAL(uploadingChanged(bool)));
    QObject::connect(dbusHandler, SIGNAL(configChanged()), uploader, SLOT(settingsChanged()));

    // We only need one instance
    if(!QDBusConnection::sessionBus().registerService("com.github.beidl.HarbourOwncloud.Daemon") ||
            !QDBusConnection::sessionBus().registerObject("/", uploader)) {
        exit(1);
    }

    netMonitor->recheckNetworks();
    uploader->settingsChanged();

    return app.exec();
}
