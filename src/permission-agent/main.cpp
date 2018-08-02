#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDBusConnection>

#include <ownclouddbusconsts.h>

#include "dbushandler.h"
#include "dialogview.h"
#include "permissionrequestqueue.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

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
