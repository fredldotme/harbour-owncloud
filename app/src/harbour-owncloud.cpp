
#include <QtQuick>
#include <sailfishapp.h>

#include "owncloudbrowser.h"
#include "entryinfo.h"
#include "settings.h"
#include "downloadmanager.h"
#include "downloadentry.h"

Q_DECLARE_METATYPE(EntryInfo*)
Q_DECLARE_METATYPE(DownloadEntry*)
Q_DECLARE_METATYPE(OwnCloudBrowser*)

int main(int argc, char *argv[])
{
    qmlRegisterType<EntryInfo>("harbour.owncloud", 1, 0, "EntryInfo");
    qmlRegisterType<OwnCloudBrowser>("harbour.owncloud", 1, 0, "OwnCloudBrowser");
    qmlRegisterType<Settings>("harbour.owncloud", 1, 0, "Settings");
    qmlRegisterType<DownloadManager>("harbour.owncloud", 1, 0, "DownloadManager");
    qmlRegisterType<DownloadEntry>("harbour.owncloud", 1, 0, "DownloadEntry");

    QGuiApplication *app = SailfishApp::application(argc, argv);
    app->setOrganizationName("harbour-owncloud");
    app->setOrganizationDomain("harbour-owncloud");
    app->setApplicationName("harbour-owncloud");
    QQuickView *view = SailfishApp::createView();

    Settings *settings = new Settings();
    OwnCloudBrowser *browser = new OwnCloudBrowser(NULL, settings);
    DownloadManager *downloader = new DownloadManager(0, browser);

    view->rootContext()->setContextProperty("browser", browser);
    view->rootContext()->setContextProperty("settings", settings);
    view->rootContext()->setContextProperty("downloader", downloader);

    view->setSource(SailfishApp::pathTo("qml/harbour-owncloud.qml"));
    view->showFullScreen();

    return app->exec();
}

