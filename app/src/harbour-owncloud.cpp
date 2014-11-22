
#include <QtQuick>
#include <sailfishapp.h>

#include "owncloudbrowser.h"
#include "entryinfo.h"
#include "settings.h"

Q_DECLARE_METATYPE(EntryInfo*)
Q_DECLARE_METATYPE(OwnCloudBrowser*)

int main(int argc, char *argv[])
{
    qmlRegisterType<EntryInfo>("OwnCloud", 1, 0, "EntryInfo");
    qmlRegisterType<OwnCloudBrowser>("OwnCloud", 1, 0, "OwnCloudBrowser");
    qmlRegisterType<Settings>("OwnCloud", 1, 0, "Settings");

    Settings *settings = new Settings();
    OwnCloudBrowser *browser = new OwnCloudBrowser(NULL, settings);

    QGuiApplication *app = SailfishApp::application(argc, argv);
    QQuickView *view = SailfishApp::createView();


    view->rootContext()->setContextProperty("browser", browser);
    view->rootContext()->setContextProperty("settings", settings);
    view->setSource(SailfishApp::pathTo("qml/harbour-owncloud.qml"));
    view->showFullScreen();

    return app->exec();
}

