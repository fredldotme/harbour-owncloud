#ifndef DBUSHANDLER_H
#define DBUSHANDLER_H

#include <QObject>
#include <ownclouddbusconsts.h>
#include <QDBusContext>
#include <QDBusMessage>

class DBusHandler : public QObject, QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", HARBOUR_OWNCLOUD_PERMD_IFACE)

public:
    explicit DBusHandler(QObject *parent = nullptr);
    ~DBusHandler();

public slots:
    void requestConfig();

signals:
    void dialogRequested(QDBusMessage requestMessage);

};

#endif // DBUSHANDLER_H
