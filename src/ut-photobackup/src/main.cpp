// Qt Core & QML
#include <QByteArray>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>
#include <QDebug>
#include <QFile>

// Common
#include <util/qappprepareutil.h>
#include <settings/db/utaccountsdb.h>

#include "daemoncontrol.h"
#include "servicecreator.h"

// Provide a fake SailfishOS namespace
// on QtQuick.Controls-based environments
namespace SailfishApp {
static QGuiApplication* application(int& argc, char** argv) {
    if (qApp)
        return static_cast<QGuiApplication*>(qApp);
    return new QGuiApplication(argc, argv);
}
}

namespace {
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

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    qmlRegisterType<DaemonControl>("harbour.owncloud", 1, 0, "DaemonControl");
    qmlRegisterType<ServiceCreator>("harbour.owncloud", 1, 0, "ServiceCreator");
    qmlRegisterType<UtAccountsDb>("harbour.owncloud", 1, 0, "AccountsDb");

    QGuiApplication* app = SailfishApp::application(argc, argv);
    prepareAppProperties(*app);

    // Create common directories
    {
        const QString upstartServiceDir =
                QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QStringLiteral("/.config/upstart");
        createNecessaryDir(upstartServiceDir);
    }

    bool gspParse = false;
    const int gspEnv = qgetenv("GRID_UNIT_PX").toInt(&gspParse);
    const int GRID_UNIT_PX = gspParse ? gspEnv : 8;

    const int headerBarSize = 48;

    QQmlApplicationEngine* newEngine = new QQmlApplicationEngine(app);
    newEngine->rootContext()->setContextProperty("headerBarSize", headerBarSize);
    newEngine->rootContext()->setContextProperty("GRID_UNIT_PX", GRID_UNIT_PX);
    newEngine->load(QUrl("qrc:/qml/main.qml"));

    return app->exec();
}

