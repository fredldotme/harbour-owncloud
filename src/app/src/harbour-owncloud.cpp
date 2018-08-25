#include <QtQuick>
#include <QDebug>
#include <QFile>
#include <sailfishapp.h>

#include <commandqueue.h>
#include <provider/storage/webdavcommandqueue.h>
#include <provider/accountinfo/ocscommandqueue.h>
#include <settings/inifilesettings.h>
#include <settings/db/accountdb.h>
#include <settings/db/syncdb.h>
#include <auth/qwebdavauthenticator.h>
#include <auth/authenticationexaminer.h>
#include <auth/flowloginauthenticator.h>
#include <util/filepathutil.h>
#include <net/thumbnailfetcher.h>
#include <net/avatarfetcher.h>
#include <localfilebrowser.h>
#include <sailfish-ui-set.h>
#include <qmlmap.h>
#include <nextcloudendpointconsts.h>
#include <cacheprovider.h>
#include "daemoncontrol.h"
#include "directorycontentmodel.h"
#include "ocsnetaccessfactory.h"
#include "webdavmediafeeder.h"
#include "accountworkers.h"
#include "accountworkergenerator.h"

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
    qmlRegisterType<IniFileSettings>("harbour.owncloud", 1, 0, "NextcloudSettings");
    qmlRegisterType<AccountDb>("harbour.owncloud", 1, 0, "AccountDb");
    qmlRegisterType<SyncDb>("harbour.owncloud", 1, 0, "SyncDb");
    qmlRegisterType<QWebDavAuthenticator>("harbour.owncloud", 1, 0, "QWebDavAuthenticator");
    qmlRegisterType<FlowLoginAuthenticator>("harbour.owncloud", 1, 0, "FlowLoginAuthenticator");
    qmlRegisterType<AuthenticationExaminer>("harbour.owncloud", 1, 0, "AuthenticationExaminer");
    qmlRegisterType<CommandEntity>("harbour.owncloud", 1, 0, "CommandEntity");
    qmlRegisterType<CommandQueue>("harbour.owncloud", 1, 0, "CommandQueue");
    qmlRegisterType<WebDavCommandQueue>("harbour.owncloud", 1, 0, "WebDavCommandQueue");
    qmlRegisterType<OcsCommandQueue>("harbour.owncloud", 1, 0, "OcsCommandQueue");
    qmlRegisterType<CacheProvider>("harbour.owncloud", 1, 0, "CacheProvider");
    qmlRegisterType<ThumbnailFetcher>("harbour.owncloud", 1, 0, "ThumbnailFetcher");
    qmlRegisterType<AvatarFetcher>("harbour.owncloud", 1, 0, "AvatarFetcher");
    qmlRegisterType<WebDavMediaFeeder>("harbour.owncloud", 1, 0, "WebDavMediaFeeder");
    qmlRegisterType<OscNetAccess>("harbour.owncloud", 1, 0, "OscNetAccess");
    qmlRegisterType<DaemonControl>("harbour.owncloud", 1, 0, "DaemonControl");
    qmlRegisterType<AccountWorkerGenerator>("harbour.owncloud", 1, 0, "AccountWorkerGenerator");
    qmlRegisterUncreatableType<AccountWorkers>("harbour.owncloud", 1, 0, "AccountWorkers",
                                               "AccountWorkers are provided through the AccountDbWorkers type");
    qmlRegisterSingletonType("harbour.owncloud", 1, 0, "FilePathUtil", filePathUtilProvider);

    QGuiApplication *app = SailfishApp::application(argc, argv);
    app->setOrganizationName("harbour-owncloud");
    app->setOrganizationDomain("harbour-owncloud");
    app->setApplicationName("harbour-owncloud");

    {
        AccountDb accounts;
        IniFileSettings iniSettings;
        if (QFile::exists(iniSettings.filePath())) {
            iniSettings.readSettings();
            const bool addAccountSuccess = accounts.addAccount(&iniSettings);
            if (!addAccountSuccess) {
                qWarning() << "Failed to migrate account";
            } else {
                const bool removeIniSuccess = QFile::remove(iniSettings.filePath());
                if (!removeIniSuccess) {
                    qWarning() << "Failed to remove obsolete .ini-based settings file";
                }
            }
        }
    }

    QQmlEngine* newEngine = new QQmlEngine;
    /*QQmlNetworkAccessManagerFactory* accessInterceptor =
            new OcsNetAccessFactory(IniFileSettings::instance());

    newEngine->setNetworkAccessManagerFactory(accessInterceptor);
    newEngine->rootContext()->setContextProperty("persistentSettings",
                                                 IniFileSettings::instance());*/

    QQuickView *view = new QQuickView(newEngine, Q_NULLPTR); //SailfishApp::createView();

    view->setSource(QUrl("qrc:/qml/harbour-owncloud.qml"));
    view->showFullScreen();

    return app->exec();
}

