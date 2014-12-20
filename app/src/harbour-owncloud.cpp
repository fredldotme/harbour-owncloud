
#include <QtQuick>
#include <sailfishapp.h>

#include "owncloudbrowser.h"
#include "entryinfo.h"
#include "settings.h"
#include "transfermanager.h"
#include "transferentry.h"
#include "localfilebrowser.h"

Q_DECLARE_METATYPE(EntryInfo*)
Q_DECLARE_METATYPE(TransferEntry*)
Q_DECLARE_METATYPE(OwnCloudBrowser*)
Q_DECLARE_METATYPE(LocalFileBrowser*)

int main(int argc, char *argv[])
{
    qmlRegisterType<EntryInfo>("harbour.owncloud", 1, 0, "EntryInfo");
    qmlRegisterType<OwnCloudBrowser>("harbour.owncloud", 1, 0, "OwnCloudBrowser");
    qmlRegisterType<Settings>("harbour.owncloud", 1, 0, "Settings");
    qmlRegisterType<TransferManager>("harbour.owncloud", 1, 0, "TransferManager");
    qmlRegisterType<TransferEntry>("harbour.owncloud", 1, 0, "TransferEntry");
    qmlRegisterType<LocalFileBrowser>("harbour.owncloud", 1, 0, "LocalFileBrowser");

    QGuiApplication *app = SailfishApp::application(argc, argv);
    app->setOrganizationName("harbour-owncloud");
    app->setOrganizationDomain("harbour-owncloud");
    app->setApplicationName("harbour-owncloud");
    QQuickView *view = SailfishApp::createView();

    Settings *settings = new Settings();
    OwnCloudBrowser *browser = new OwnCloudBrowser(NULL, settings);
    TransferManager *transfer = new TransferManager(0, browser);

    view->rootContext()->setContextProperty("browser", browser);
    view->rootContext()->setContextProperty("settings", settings);
    view->rootContext()->setContextProperty("transfer", transfer);

    view->setSource(SailfishApp::pathTo("qml/harbour-owncloud.qml"));
    view->showFullScreen();

    return app->exec();
}

