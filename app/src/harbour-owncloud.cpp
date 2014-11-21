
#include <QtQuick>
#include <sailfishapp.h>

#include "owncloudbrowser.h"
#include "entryinfo.h"


Q_DECLARE_METATYPE(EntryInfo*)

int main(int argc, char *argv[])
{
    qmlRegisterType<EntryInfo>();
    qmlRegisterType<OwnCloudBrowser>();

    OwnCloudBrowser *browser = new OwnCloudBrowser();

    QGuiApplication *app = SailfishApp::application(argc, argv);
    QQuickView *view = SailfishApp::createView();

    view->setSource(SailfishApp::pathTo("qml/harbour-owncloud.qml"));

    view->rootContext()->setContextProperty("browser", browser);
    view->showFullScreen();

    browser->getDirectoryContent("/");

    return app->exec();
}

