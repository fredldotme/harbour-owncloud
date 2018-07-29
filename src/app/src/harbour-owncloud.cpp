#include <QtQuick>
#include <sailfishapp.h>

#include <commandqueue.h>
#include <webdavcommandqueue.h>
#include <ocscommandqueue.h>
#include <settings/nextcloudsettings.h>
#include <settings/examinationsettings.h>
#include <auth/qwebdavauthenticator.h>
#include <auth/authenticationexaminer.h>
#include <auth/flowloginauthenticator.h>
#include <util/filepathutil.h>
#include <net/thumbnailfetcher.h>
#include <localfilebrowser.h>
#include <sailfish-ui-set.h>
#include <qmlmap.h>
#include <nextcloudendpointconsts.h>
#include "daemoncontrol.h"
#include "directorycontentmodel.h"
#include "ocsnetaccessfactory.h"
#include "webdavmediafeeder.h"

static QJSValue filePathUtilProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)

    QJSValue filePathUtilObj = scriptEngine->newQObject(new FilePathUtil);
    filePathUtilObj.setProperty("FilePathUtil", filePathUtilObj);
    return filePathUtilObj;
}

int main(int argc, char *argv[])
{
    SailfishUiSet::registerQmlTypes();
    qmlRegisterType<QmlMap>("harbour.owncloud", 1, 0, "QmlMap");
    qmlRegisterType<DirectoryContentModel>("harbour.owncloud", 1, 0, "DirectoryContentModel");
    qmlRegisterType<NextcloudSettings>("harbour.owncloud", 1, 0, "NextcloudSettings");
    qmlRegisterType<QWebDavAuthenticator>("harbour.owncloud", 1, 0, "QWebDavAuthenticator");
    qmlRegisterType<FlowLoginAuthenticator>("harbour.owncloud", 1, 0, "FlowLoginAuthenticator");
    qmlRegisterType<AuthenticationExaminer>("harbour.owncloud", 1, 0, "AuthenticationExaminer");
    qmlRegisterType<CommandEntity>("harbour.owncloud", 1, 0, "CommandEntity");
    qmlRegisterType<CommandQueue>("harbour.owncloud", 1, 0, "CommandQueue");
    qmlRegisterType<WebDavCommandQueue>("harbour.owncloud", 1, 0, "WebDavCommandQueue");
    qmlRegisterType<OcsCommandQueue>("harbour.owncloud", 1, 0, "OcsCommandQueue");
    qmlRegisterType<ThumbnailFetcher>("harbour.owncloud", 1, 0, "ThumbnailFetcher");
    qmlRegisterType<WebDavMediaFeeder>("harbour.owncloud", 1, 0, "WebDavMediaFeeder");
    qmlRegisterType<OscNetAccess>("harbour.owncloud", 1, 0, "OscNetAccess");
    qmlRegisterType<DaemonControl>("harbour.owncloud", 1, 0, "DaemonControl");
    qmlRegisterSingletonType("harbour.owncloud", 1, 0, "FilePathUtil", filePathUtilProvider);

    QGuiApplication *app = SailfishApp::application(argc, argv);
    app->setOrganizationName("harbour-owncloud");
    app->setOrganizationDomain("harbour-owncloud");
    app->setApplicationName("harbour-owncloud");

    NextcloudSettings::instance()->readSettings();

    QQmlEngine* newEngine = new QQmlEngine;
    QQmlNetworkAccessManagerFactory* accessInterceptor =
            new OcsNetAccessFactory(NextcloudSettings::instance());

    newEngine->setNetworkAccessManagerFactory(accessInterceptor);
    newEngine->rootContext()->setContextProperty("persistentSettings",
                                                 NextcloudSettings::instance());

    QQuickView *view = new QQuickView(newEngine, Q_NULLPTR); //SailfishApp::createView();

    view->setSource(QUrl("qrc:/qml/harbour-owncloud.qml"));
    view->showFullScreen();

    return app->exec();
}

