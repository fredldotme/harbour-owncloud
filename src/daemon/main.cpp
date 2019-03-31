#define DEBUG_WEBDAV 1

#include <QCoreApplication>
#include <QDBusConnection>
#include <QFile>
#include <QTimer>

#include <ownclouddbusconsts.h>

#include "filesystem.h"
#include "settings/inifilesettings.h"
#include "dbushandler.h"
#include "networkmonitor.h"
#include "uploader.h"

#include <commands/sync/ncdirtreecommandunit.h>
#include <commands/sync/ncsynccommandunit.h>

QString remoteDirectoryFromHwRelease()
{
    QFile hwRelease("/etc/hw-release");
    if (hwRelease.open(QFile::ReadOnly)) {
        QString allLines = QString(hwRelease.readAll());
        hwRelease.close();
        QStringList lineArray = allLines.split("\n");
        for (QString& line : lineArray) {
            if (!line.startsWith("NAME="))
                continue;

            // Replace characters which would be invalid
            // within a WebDav directory name
            return NODE_PATH_SEPARATOR +
                    line.replace("NAME=", "")
                    .replace("\"", "")
                    .replace("/", "_")
                    .trimmed() + "/";

        }
    }
    return QStringLiteral("/Jolla/");
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    IniFileSettings* settings = IniFileSettings::instance();

    // Status updates and config reload requests through DBus
    DBusHandler *dbusHandler = new DBusHandler();

    // We only need one instance
    if(!QDBusConnection::sessionBus().registerService(HarbourOwncloud::DBusConsts::DBUS_SERVICE) ||
            !QDBusConnection::sessionBus().registerObject(HarbourOwncloud::DBusConsts::DBUS_PATH, dbusHandler,
                                                          QDBusConnection::ExportAllSlots |
                                                          QDBusConnection::ExportAllProperties |
                                                          QDBusConnection::ExportAllSignals)) {
        exit(1);
    }

    const QString targetDirectory = remoteDirectoryFromHwRelease();

    Filesystem *fsHandler = Filesystem::instance();
    NetworkMonitor *netMonitor = NetworkMonitor::instance(settings);
    Uploader* uploader = new Uploader(&app, targetDirectory, netMonitor, settings);

    // Periodically check for existance of the local pictures path until found.
    // Don't stop the timer as the external storage could be ejected anytime.
    // Every 10 minutes should be enough in this specific case.
    QDir localPath(settings->localPicturesPath());
    QTimer localPathCheck;
    localPathCheck.setInterval(60000 * 10);
    localPathCheck.setSingleShot(false);
    QObject::connect(&localPathCheck, &QTimer::timeout, &localPathCheck,
                     [&localPath, fsHandler]() {
        if (!localPath.exists())
            return;

        fsHandler->triggerRescan();
    });
    localPathCheck.start();

    QObject::connect(fsHandler, &Filesystem::fileFound, [uploader](QString fullPath){
        Q_UNUSED(fullPath)
        uploader->triggerSync();
    });

    // DBus connections
    QObject::connect(dbusHandler, &DBusHandler::configReloadRequested,
                     settings, &IniFileSettings::readSettings,
                     Qt::QueuedConnection);
    QObject::connect(dbusHandler, &DBusHandler::abortRequested, uploader, &Uploader::stopSync);

    QObject::connect(netMonitor, &NetworkMonitor::shouldSyncChanged, [uploader, fsHandler] (bool shouldSync) {
        if (!(uploader && fsHandler)) {
            qCritical() << "Invalid object existance (uploader, fsHandler), bailing out.";
            return;
        }

        fsHandler->inhibitScan(!uploader->shouldSync());

        if (shouldSync) {
            uploader->triggerSync();
        } else {
            uploader->stopSync();
        }
    });

    QObject::connect(uploader, &Uploader::runningChanged, [dbusHandler, uploader]() {
        if (!(dbusHandler && uploader)) {
            qCritical() << "Invalid object existance (dbusHandler, uploader), bailing out.";
            return;
        }
        dbusHandler->setUploading(uploader->running());
    });

    QObject::connect(settings, &IniFileSettings::uploadAutomaticallyChanged,
                     &app, [uploader, fsHandler](){
        if (!(uploader && fsHandler)) {
            qCritical() << "Invalid object existance (uploader, fsHandler), bailing out.";
            return;
        }
        fsHandler->inhibitScan(!uploader->shouldSync());
        uploader->triggerSync();
    }, Qt::QueuedConnection);

    settings->readSettings();
    netMonitor->recheckNetworks();
    fsHandler->inhibitScan(!uploader->shouldSync());
    fsHandler->triggerRescan();

    return app.exec();
}
