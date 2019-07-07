#define DEBUG_WEBDAV 1

#include <QCoreApplication>
#include <QDBusConnection>
#include <QFile>
#include <QTimer>

#include <ownclouddbusconsts.h>
#include <util/qappprepareutil.h>

#include "filesystem.h"
#include "settings/inifilesettings.h"
#include "dbushandler.h"
#include "networkmonitor.h"
#include "uploader.h"

#include <commands/sync/ncdirtreecommandunit.h>
#include <commands/sync/ncsynccommandunit.h>
#include <accountworkergenerator.h>
#include <settings/db/accountdb.h>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

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
                    .trimmed();

        }
    }
#ifdef GHOSTCLOUD_UBUNTU_TOUCH
    return QStringLiteral("/GhostCloud");
#else
    return QStringLiteral("/Jolla");
#endif
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    prepareAppProperties(app);

    qInfo() << "Starting harbour-owncloud-daemon";

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

#ifdef Q_OS_UNIX
    // Re-exec on reload request
    QObject::connect(dbusHandler, &DBusHandler::configReloadRequested,
                     &app, [=]() {
        execv(argv[0], argv);
    });
#endif

    const QString targetDirectory = remoteDirectoryFromHwRelease();

    AccountWorkerGenerator generator;
    AccountDb accountDatabase;
    generator.setDatabase(&accountDatabase);

    for (const QVariant& accountWorkerVariant : generator.accountWorkers()) {
        QObject* workersReference = qvariant_cast<QObject *>(accountWorkerVariant);
        if (!workersReference) {
            qWarning() << "'workersReference' is invalid, skipping...";
            continue;
        }

        AccountWorkers* workers = qobject_cast<AccountWorkers*>(workersReference);
        if (!workers) {
            qWarning() << "'workers' is invalid, skipping...";
            continue;
        }

        AccountBase* settings = workers->account();

        if (!settings->uploadAutomatically())
            continue;

        qInfo() << "Photo backup enabled for account" << settings->hoststring();

        NetworkMonitor *netMonitor = new NetworkMonitor(workers, settings);
        Filesystem *fsHandler = new Filesystem(settings);
        Uploader* uploader = new Uploader(&app, targetDirectory, netMonitor, settings);

        // Periodically check for existence of the local pictures path until found.
        // Don't stop the timer as the external storage could be ejected anytime.
        // Every 10 minutes should be enough in this specific case.
        QTimer localPathCheck;
        localPathCheck.setInterval(60000 * 10);
        localPathCheck.setSingleShot(false);
        QObject::connect(&localPathCheck, &QTimer::timeout, &localPathCheck,
                         [fsHandler]() {

            fsHandler->triggerRescan();
        });
        localPathCheck.start();

        QObject::connect(fsHandler, &Filesystem::locationFound, uploader, &Uploader::triggerSync);

        // listen for new files
        // NOTE: this signal is triggered for every file on filesystem rescan,
        QObject::connect(fsHandler, &Filesystem::fileFound, [uploader](QString locationDir, QString locationName, QString fullPath){
            Q_UNUSED(fullPath)
            uploader->triggerSync(locationDir, locationName);
        });

        // DBus connections
        QObject::connect(dbusHandler, &DBusHandler::abortRequested, uploader, &Uploader::stopSync);

        QObject::connect(netMonitor, &NetworkMonitor::shouldSyncChanged, [uploader, fsHandler] (bool shouldSync) {
            if (!(uploader && fsHandler)) {
                qCritical() << "Invalid object existance (uploader, fsHandler), bailing out.";
                return;
            }

            fsHandler->inhibitScan(!uploader->shouldSync());

            if (shouldSync) {
                fsHandler->triggerRescan();
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

        QObject::connect(settings, &AccountBase::uploadAutomaticallyChanged,
                         &app, [uploader, fsHandler](){
            if (!(uploader && fsHandler)) {
                qCritical() << "Invalid object existance (uploader, fsHandler), bailing out.";
                return;
            }
            fsHandler->inhibitScan(!uploader->shouldSync());
            fsHandler->triggerRescan();
        }, Qt::QueuedConnection);

        netMonitor->recheckNetworks();
        fsHandler->inhibitScan(!uploader->shouldSync());
        fsHandler->triggerRescan();
    }

    return app.exec();
}
