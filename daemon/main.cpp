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

    Filesystem fsHandler;
    Uploader uploader;
    DBusHandler * dbusHandler = new DBusHandler(&uploader);
    NetworkMonitor netMonitor;
    QObject::connect(&fsHandler, SIGNAL(fileFound(QString)), &uploader, SLOT(fileFound(QString)));
    QObject::connect(&uploader, SIGNAL(localPathUpdated()), &fsHandler, SLOT(localPathChanged()));
    QObject::connect(&netMonitor, SIGNAL(shouldDownloadChanged(bool)), &uploader, SLOT(onlineChanged(bool)));

    // DBus connections
    QObject::connect(dbusHandler, SIGNAL(suspendedChanged(bool)), &uploader, SLOT(setSuspended(bool)));
    QObject::connect(&netMonitor, SIGNAL(shouldDownloadChanged(bool)), dbusHandler, SLOT(setOffline(bool)));
    QObject::connect(&uploader, SIGNAL(fileUploaded(QString)), dbusHandler, SIGNAL(fileUploaded(QString)));
    QObject::connect(&uploader, SIGNAL(connectError(QString)), dbusHandler, SIGNAL(connectError(QString)));
    QObject::connect(&uploader, SIGNAL(uploadError(QString)), dbusHandler, SIGNAL(uploadError(QString)));
    QObject::connect(&uploader, SIGNAL(uploadingChanged(bool)), dbusHandler, SIGNAL(uploadingChanged(bool)));
    QObject::connect(dbusHandler, SIGNAL(configChanged()), &fsHandler, SLOT(localPathChanged()));
    QObject::connect(dbusHandler, SIGNAL(configChanged()), &uploader, SLOT(settingsChanged()));

    // We only need one instance
    if(!QDBusConnection::sessionBus().registerService("com.github.beidl.HarbourOwncloud.Daemon") ||
            !QDBusConnection::sessionBus().registerObject("/", &uploader)) {
        exit(1);
    }

    return app.exec();
}
