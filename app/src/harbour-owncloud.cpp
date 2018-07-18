#include <QtQuick>
#include <sailfishapp.h>
#include <notification.h>

#include <commandqueue.h>
#include <settings/nextcloudsettings.h>
#include <net/owncloudbrowser.h>
#include <net/transfermanager.h>
#include <net/transferentry.h>
#include <net/thumbnailfetcher.h>
#include <entryinfo.h>
#include <localfilebrowser.h>
#include <sailfish-ui-set.h>
#include "daemoncontrol.h"

int main(int argc, char *argv[])
{
    SailfishUiSet::registerQmlTypes();
    qmlRegisterType<RemoteFileInfo>("harbour.owncloud", 1, 0, "RemoteFileInfo");
    qmlRegisterType<CommandEntity>("harbour.owncloud", 1, 0, "CommandEntity");
    qmlRegisterType<CommandQueue>("harbour.owncloud", 1, 0, "CommandQueue");
    qmlRegisterType<ThumbnailFetcher>("harbour.owncloud", 1, 0, "ThumbnailFetcher");
    qmlRegisterType<OwnCloudBrowser>("harbour.owncloud", 1, 0, "OwnCloudBrowser");
    qmlRegisterType<TransferManager>("harbour.owncloud", 1, 0, "TransferManager");
    qmlRegisterType<TransferEntry>("harbour.owncloud", 1, 0, "TransferEntry");
    qmlRegisterType<DaemonControl>("harbour.owncloud", 1, 0, "DaemonControl");
    qmlRegisterType<Notification>("harbour.owncloud", 1, 0, "Notification");

    QGuiApplication *app = SailfishApp::application(argc, argv);
    app->setOrganizationName("harbour-owncloud");
    app->setOrganizationDomain("harbour-owncloud");
    app->setApplicationName("harbour-owncloud");
    QQuickView *view = SailfishApp::createView();

    NextcloudSettings *settings = NextcloudSettings::instance();
    OwnCloudBrowser *browser = new OwnCloudBrowser(NULL, settings);
    TransferManager *transfer = new TransferManager(0, browser);
    DaemonControl *daemonCtrl = new DaemonControl();

    view->rootContext()->setContextProperty("browser", browser);
    view->rootContext()->setContextProperty("settings", settings);
    view->rootContext()->setContextProperty("transfer", transfer);
    view->rootContext()->setContextProperty("daemonCtrl", daemonCtrl);

    view->setSource(SailfishApp::pathTo("qml/harbour-owncloud.qml"));
    view->showFullScreen();

    return app->exec();
}

