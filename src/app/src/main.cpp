// Qt Core & QML
#include <QGuiApplication>
#include <QtQuick>
#include <QDebug>
#include <QFile>

// Contained in both Sailfish- and QML-UI-Set
#include <localfilebrowser.h>

// Platform specific headers
#ifndef GHOSTCLOUD_UI_QUICKCONTROLS
#include <sailfishapp.h>
#include <sailfish-ui-set.h>
#else
#include <qml-ui-set.h>
#endif

// Common library
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
#include <qmlmap.h>
#include <nextcloudendpointconsts.h>
#include <cacheprovider.h>

// Platform-specific functionality
#if defined(Q_OS_ANDROID) || defined(Q_OS_MAC) || defined(Q_OS_IOS)
#include "daemonctrl/dummydaemonctrl.h"
#elif defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID)
#include "daemonctrl/daemoncontrol.h"
#endif

// Android intent file browsing capability
#if defined(Q_OS_ANDROID)
#include <QtAndroid>
#include "os/android/intentfileselector.h"
#else
#include "os/dummy/dummyintentfileselector.h"
#endif

#include "directorycontentmodel.h"
#include "ocsnetaccessfactory.h"
#include "webdavmediafeeder.h"
#include "accountworkers.h"
#include "accountworkergenerator.h"

// Provide a fake SailfishOS namespace
// on QtQuick.Controls-based environments
#ifdef GHOSTCLOUD_UI_QUICKCONTROLS
namespace SailfishApp {
static QGuiApplication* application(int& argc, char** argv) {
    if (qApp)
        return static_cast<QGuiApplication*>(qApp);
    return new QGuiApplication(argc, argv);
}
}
#endif

namespace {
static QJSValue filePathUtilProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)

    QJSValue filePathUtilObj = scriptEngine->newQObject(new FilePathUtil);
    filePathUtilObj.setProperty("FilePathUtil", filePathUtilObj);
    return filePathUtilObj;
}

inline static void createNecessaryDir(const QString& path) {
    const QDir configDir = QDir(path);
    if (!configDir.exists()) {
        const auto success = configDir.mkpath(configDir.absolutePath());
        if (success)
            return;

        qWarning() << "Failed to create" << configDir.absolutePath();
        return;
    }
}
}

#ifdef Q_OS_ANDROID
bool checkAndroidStoragePermissions() {
    const auto permissionsRequest = QStringList(
    { QString("android.permission.READ_EXTERNAL_STORAGE"),
      QString("android.permission.WRITE_EXTERNAL_STORAGE")});

    if (QtAndroid::checkPermission(permissionsRequest[0])
            == QtAndroid::PermissionResult::Denied
            || (QtAndroid::checkPermission(permissionsRequest[1]))
            == QtAndroid::PermissionResult::Denied) {
        auto permissionResults
                = QtAndroid::requestPermissionsSync(permissionsRequest);
        if ((permissionResults[permissionsRequest[0]]
             == QtAndroid::PermissionResult::Denied)
                || (permissionResults[permissionsRequest[1]]
                    == QtAndroid::PermissionResult::Denied))
            return false;
    }
    return true;
}
#endif

enum GCTargetOs {
    GENERIC = 0,
    ANDROID_OS,
    IOS,
    UBUNTU_TOUCH
};

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#ifndef GHOSTCLOUD_UI_QUICKCONTROLS
    SailfishUiSet::registerQmlTypes();
    const QString QHOSTCLOUD_APP_NAME = QStringLiteral("harbour-owncloud");
#else
    QmlUiSet::registerQmlTypes();
    const QString QHOSTCLOUD_APP_NAME = QStringLiteral("GhostCloud");
#endif

#ifdef Q_OS_ANDROID
    QAndroidJniObject::callStaticObjectMethod(
        "me/fredl/ghostcloud/ShareUtil",
        "setupVmPolicy",
        "()Ljava/lang/String;");

#endif

    qmlRegisterType<QmlMap>("harbour.owncloud", 1, 0, "QmlMap");
    qmlRegisterType<DirectoryContentModel>("harbour.owncloud", 1, 0, "DirectoryContentModel");
    qmlRegisterType<NextcloudSettingsBase>("harbour.owncloud", 1, 0, "NextcloudSettingsBase");
    qmlRegisterType<IniFileSettings>("harbour.owncloud", 1, 0, "NextcloudSettings");
    qmlRegisterType<AccountDb>("harbour.owncloud", 1, 0, "AccountDb");
    qmlRegisterType<SyncDb>("harbour.owncloud", 1, 0, "SyncDb");
    qmlRegisterType<QWebDavAuthenticator>("harbour.owncloud", 1, 0, "QWebDavAuthenticator");
    qmlRegisterType<FlowLoginAuthenticator>("harbour.owncloud", 1, 0, "FlowLoginAuthenticator");
    qmlRegisterType<AuthenticationExaminer>("harbour.owncloud", 1, 0, "AuthenticationExaminer");
    qmlRegisterType<CommandEntity>("harbour.owncloud", 1, 0, "CommandEntity");
    qmlRegisterType<CommandQueue>("harbour.owncloud", 1, 0, "CommandQueue");
    qmlRegisterType<CloudStorageProvider>("harbour.owncloud", 1, 0, "CloudStorageProvider");
    qmlRegisterType<WebDavCommandQueue>("harbour.owncloud", 1, 0, "WebDavCommandQueue");
    qmlRegisterType<OcsCommandQueue>("harbour.owncloud", 1, 0, "OcsCommandQueue");
    qmlRegisterType<CacheProvider>("harbour.owncloud", 1, 0, "CacheProvider");
    qmlRegisterType<ThumbnailFetcher>("harbour.owncloud", 1, 0, "ThumbnailFetcher");
    qmlRegisterType<AvatarFetcher>("harbour.owncloud", 1, 0, "AvatarFetcher");
    qmlRegisterType<WebDavMediaFeeder>("harbour.owncloud", 1, 0, "WebDavMediaFeeder");
    qmlRegisterType<OscNetAccess>("harbour.owncloud", 1, 0, "OscNetAccess");
    qmlRegisterType<DaemonControl>("harbour.owncloud", 1, 0, "DaemonControl");
    qmlRegisterType<AccountWorkerGenerator>("harbour.owncloud", 1, 0, "AccountWorkerGenerator");
#if defined (Q_OS_ANDROID)
    qmlRegisterType<IntentFileSelector>("harbour.owncloud", 1, 0, "NativeFileSelector");
#else
    qmlRegisterType<DummyIntentFileSelector>("harbour.owncloud", 1, 0, "NativeFileSelector");
#endif
    qmlRegisterUncreatableType<AccountWorkers>("harbour.owncloud", 1, 0, "AccountWorkers",
                                               "AccountWorkers are provided through the AccountDbWorkers type");
    qmlRegisterSingletonType("harbour.owncloud", 1, 0, "FilePathUtil", filePathUtilProvider);

    QGuiApplication* app = SailfishApp::application(argc, argv);

#ifdef GHOSTCLOUD_UBUNTU_TOUCH
    app->setApplicationName(QStringLiteral("me.fredl.ghostcloud"));
#else
    app->setOrganizationName(QHOSTCLOUD_APP_NAME);
    app->setOrganizationDomain(QHOSTCLOUD_APP_NAME);
    app->setApplicationName(QHOSTCLOUD_APP_NAME);
#endif

    QIcon::setThemeName("theme");

    {
        createNecessaryDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
        createNecessaryDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    }

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


#if defined(Q_OS_ANDROID)
    // Keep asking for file access permissions
    while(!checkAndroidStoragePermissions());

    const int headerBarSize = 48;
    const GCTargetOs targetOs = GCTargetOs::ANDROID_OS;
#elif defined(Q_OS_IOS)
    const int headerBarSize = 48;
    const GCTargetOs targetOs = GCTargetOs::IOS;
#elif defined(GHOSTCLOUD_UBUNTU_TOUCH)
    const int headerBarSize = 48;
    const GCTargetOs targetOs = GCTargetOs::UBUNTU_TOUCH;
#else
    const int headerBarSize = 38;
    const GCTargetOs targetOs = GCTargetOs::GENERIC;
#endif

#ifndef GHOSTCLOUD_UI_QUICKCONTROLS
    QQmlEngine* newEngine = new QQmlEngine;
    QQuickView *view = new QQuickView(newEngine, Q_NULLPTR); //SailfishApp::createView();

    view->setSource(QUrl("qrc:/qml/sfos/harbour-owncloud.qml"));
    view->showFullScreen();
#else
    QQmlApplicationEngine* newEngine = new QQmlApplicationEngine(app);
    newEngine->rootContext()->setContextProperty("headerBarSize", headerBarSize);
    newEngine->rootContext()->setContextProperty("targetOs", targetOs);
    newEngine->load(QUrl("qrc:/qml/qqc/main.qml"));
#endif

    return app->exec();
}

