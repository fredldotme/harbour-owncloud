
#include <QtQuick>
#include <sailfishapp.h>

#include "owncloudbrowser.h"
#include "entryinfo.h"
#include "settings.h"
#include "transfermanager.h"
#include "transferentry.h"
#include "localfilebrowser.h"
#include "daemoncontrol.h"

#include <notification.h>

Q_DECLARE_METATYPE(EntryInfo*)
Q_DECLARE_METATYPE(TransferEntry*)
Q_DECLARE_METATYPE(OwnCloudBrowser*)
Q_DECLARE_METATYPE(LocalFileBrowser*)

int main(int argc, char *argv[])
{
    qmlRegisterType<EntryInfo>("harbour.owncloud", 1, 0, "EntryInfo");
    qmlRegisterType<OwnCloudBrowser>("harbour.owncloud", 1, 0, "OwnCloudBrowser");
    qmlRegisterType<TransferManager>("harbour.owncloud", 1, 0, "TransferManager");
    qmlRegisterType<TransferEntry>("harbour.owncloud", 1, 0, "TransferEntry");
    qmlRegisterType<LocalFileBrowser>("harbour.owncloud", 1, 0, "LocalFileBrowser");
    qmlRegisterType<DaemonControl>("harbour.owncloud", 1, 0, "DaemonControl");
    qmlRegisterType<Notification>("harbour.owncloud", 1, 0, "Notification");

    QGuiApplication *app = SailfishApp::application(argc, argv);
    app->setOrganizationName("harbour-owncloud");
    app->setOrganizationDomain("harbour-owncloud");
    app->setApplicationName("harbour-owncloud");
    QQuickView *view = SailfishApp::createView();

    Settings *settings = Settings::instance();
    OwnCloudBrowser *browser = new OwnCloudBrowser(NULL, settings);
    TransferManager *transfer = new TransferManager(0, browser);
    DaemonControl *daemonCtrl = new DaemonControl();

    QObject::connect(settings, SIGNAL(settingsChanged()), daemonCtrl, SLOT(reloadConfig()));
    view->rootContext()->setContextProperty("browser", browser);
    view->rootContext()->setContextProperty("settings", settings);
    view->rootContext()->setContextProperty("transfer", transfer);
    view->rootContext()->setContextProperty("daemonCtrl", daemonCtrl);

    view->setSource(SailfishApp::pathTo("qml/harbour-owncloud.qml"));
    view->showFullScreen();

    return app->exec();
}

