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
    DBusHandler *dbusHandler = new DBusHandler(&app);
    NetworkMonitor netMonitor;
    QObject::connect(&fsHandler, SIGNAL(fileFound(QString)), &uploader, SLOT(fileFound(QString)));
    QObject::connect(&uploader, SIGNAL(localPathUpdated()), &fsHandler, SLOT(localPathChanged()));
    QObject::connect(&netMonitor, SIGNAL(shouldDownloadChanged(bool)), &uploader, SLOT(onlineChanged(bool)));
    QObject::connect(dbusHandler, SIGNAL(suspendedChanged(bool)), &uploader, SLOT(setSuspended(bool)));
    QObject::connect(&netMonitor, SIGNAL(shouldDownloadChanged(bool)), dbusHandler, SLOT(setOffline(bool)));
    QObject::connect(&uploader, SIGNAL(fileUploaded(QString)), dbusHandler, SIGNAL(fileUploaded(QString)));
    QObject::connect(&uploader, SIGNAL(connectError(QString)), dbusHandler, SIGNAL(connectError(QString)));
    QObject::connect(&uploader, SIGNAL(uploadError(QString)), dbusHandler, SIGNAL(uploadError(QString)));

    QDBusConnection::sessionBus().registerObject("/HarbourOwncloudDaemon", dbusHandler,
                                                 QDBusConnection::ExportAllProperties |
                                                 QDBusConnection::ExportAllInvokables |
                                                 QDBusConnection::ExportAllSignals);

    // assert for poor man's single instance application
    Q_ASSERT(QDBusConnection::sessionBus().registerService("com.github.beidl.harbour-owncloud.daemon"));


    return app.exec();
}
