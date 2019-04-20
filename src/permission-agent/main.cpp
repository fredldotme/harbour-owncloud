#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDBusConnection>

#include <ownclouddbusconsts.h>
#include <util/qappprepareutil.h>

#include <accountworkergenerator.h>
#include <accountworkers.h>
#include <settings/db/accountdb.h>

#include "dbushandler.h"
#include "dialogview.h"
#include "permissionrequestqueue.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<AccountDb>("harbour.owncloud", 1, 0, "AccountDb");
    qmlRegisterType<AccountWorkerGenerator>("harbour.owncloud", 1, 0, "AccountWorkerGenerator");
    qmlRegisterUncreatableType<AccountWorkers>("harbour.owncloud", 1, 0, "AccountWorkers",
                                               "AccountWorkers are provided through the AccountDbWorkers type");

    QGuiApplication app(argc, argv);
    prepareAppProperties(app);

    DBusHandler *dbusHandler = new DBusHandler;
    PermissionRequestQueue *requestQueue = new PermissionRequestQueue;
    QObject::connect(dbusHandler, &DBusHandler::dialogRequested,
                     requestQueue, &PermissionRequestQueue::enqueueRequest);

    // We only need one instance
    if(!QDBusConnection::sessionBus().registerService(HarbourOwncloud::DBusConsts::PermAgent::DBUS_SERVICE) ||
            !QDBusConnection::sessionBus().registerObject(HarbourOwncloud::DBusConsts::PermAgent::DBUS_PATH, dbusHandler,
                                                          QDBusConnection::ExportAllSlots)) {
        exit(1);
    }

    DialogView* permissionsDialog = new DialogView();
    permissionsDialog->engine()->rootContext()->setContextProperty("queue", requestQueue);
    permissionsDialog->setSource(QUrl("qrc:/PermissionsDialog.qml"));
    permissionsDialog->setFlags(Qt::Dialog | Qt::FramelessWindowHint);

    // Show and raise dialog on a request
    QObject::connect(requestQueue, &PermissionRequestQueue::permissionRequested, permissionsDialog, &QQuickView::show);
    QObject::connect(requestQueue, &PermissionRequestQueue::permissionRequested, permissionsDialog, &QQuickView::raise);

    return app.exec();
}
