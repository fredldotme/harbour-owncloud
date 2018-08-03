#define DEBUG_WEBDAV 1

#include <QCoreApplication>
#include <QDBusConnection>
#include <QFile>
#include <QTimer>

#include <ownclouddbusconsts.h>

#include <webdavcommandqueue.h>
#include "filesystem.h"
#include "settings/nextcloudsettings.h"
#include "dbushandler.h"
#include "networkmonitor.h"

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
            if (line.startsWith("NAME=")) {

                // Replace characters which would be invalid
                // within a WebDav directory name
                return QStringLiteral("/") +
                        line.replace("NAME=", "")
                        .replace("\"", "")
                        .replace("/", "_")
                        .trimmed() + "/";
            }
        }
    }
    return QStringLiteral("/Jolla/");
}

inline void enqueueSyncCommand(WebDavCommandQueue* webDavQueue,
                               NextcloudSettings* settings,
                               QString remoteTarget)
{
    if (!(webDavQueue && settings)) {
        qCritical() << "Invalid object existance (webDavQueue, settings), bailing out.";
        return;
    }

    if (!settings->uploadAutomatically())
        return;

    if (webDavQueue->queue().length() > 0)
        return;

    NcSyncCommandUnit* syncDirectoriesUnit = new NcSyncCommandUnit(webDavQueue,
                                                                   webDavQueue->getWebdav(),
                                                                   settings->localPicturesPath(),
                                                                   remoteTarget);
    webDavQueue->enqueue((CommandEntity*)syncDirectoriesUnit);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    NextcloudSettings* settings = NextcloudSettings::instance();
    settings->readSettings();

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
    NetworkMonitor *netMonitor = NetworkMonitor::instance(NextcloudSettings::instance());

    WebDavCommandQueue* webDavQueue = new WebDavCommandQueue(&app, settings);
    webDavQueue->setImmediate(true);

    // Periodically check for existance of the local pictures path until found.
    // Don't stop the timer as the external storage could be ejected anytime.
    // Every 10 minutes should be enough in this specific case.
    QDir localPath(settings->localPicturesPath());
    QTimer localPathCheck;
    localPathCheck.setInterval(60000 * 10);
    localPathCheck.setSingleShot(false);
    QObject::connect(&localPathCheck, &QTimer::timeout, &localPathCheck,
                     [&localPath, &localPathCheck, fsHandler]() {
        if (localPath.exists()) fsHandler->triggerRescan();
    });
    localPathCheck.start();

    QObject::connect(fsHandler, &Filesystem::fileFound,
                     [fsHandler, webDavQueue, settings, netMonitor, targetDirectory]
                     (QString fullPath){
        if (!(fsHandler && webDavQueue && settings && netMonitor)) {
            qCritical() << "Invalid object existance (fsHandler, webDavQueue, settings, netMonitor)"
                        << ", bailing out.";
            return;
        }

        if (!netMonitor->shouldDownload())
            return;

        qDebug() << fullPath;
        enqueueSyncCommand(webDavQueue, settings, targetDirectory);
    });

    // DBus connections
    QObject::connect(dbusHandler, &DBusHandler::abortRequested, webDavQueue, &WebDavCommandQueue::stop);
    QObject::connect(dbusHandler, &DBusHandler::configReloadRequested, settings, &NextcloudSettings::readSettings);

    QObject::connect(netMonitor, &NetworkMonitor::shouldDownloadChanged, [netMonitor, dbusHandler, webDavQueue](){
        if (!(netMonitor && dbusHandler && webDavQueue)) {
            qCritical() << "Invalid object existance (netMonitor, dbusHandler, webDavQueue), bailing out.";
            return;
        }
        dbusHandler->setOnline(netMonitor->shouldDownload());
        if (!netMonitor->shouldDownload() && webDavQueue->isRunning()) {
            webDavQueue->stop();
            return;
        }

        if (!netMonitor->shouldDownload())
            return;

        qDebug() << fullPath;
        enqueueSyncCommand(webDavQueue, settings, targetDirectory);
    });

    QObject::connect(webDavQueue, &WebDavCommandQueue::runningChanged, [dbusHandler, webDavQueue]() {
        if (!(dbusHandler && webDavQueue)) {
            qCritical() << "Invalid object existance (dbusHandler, webDavQueue), bailing out.";
            return;
        }
        dbusHandler->setUploading(webDavQueue->isRunning());
    });

    //QObject::connect(uploader, &Uploader::fileUploaded, dbusHandler, &DBusHandler::fileUploaded);
    //QObject::connect(uploader, &Uploader::connectError, dbusHandler, &DBusHandler::connectError);
    //QObject::connect(uploader, &Uploader::uploadError, dbusHandler, &DBusHandler::uploadError);

    QObject::connect(settings, &NextcloudSettings::uploadAutomaticallyChanged,
                     [webDavQueue, netMonitor, dbusHandler]() {
        if (!netMonitor || !webDavQueue) {
            qCritical() << "Invalid object existance (netMonitor, webDavQueue), bailing out.";
            return;
        }
        dbusHandler->setOnline(netMonitor->shouldDownload());
        if (!netMonitor->shouldDownload() && webDavQueue->isRunning())
            webDavQueue->stop();
    });

    netMonitor->recheckNetworks();
    dbusHandler->setOnline(netMonitor->shouldDownload());
    fsHandler->triggerRescan();

    return app.exec();
}
