#ifndef OWNCLOUDDBUSCONSTS_H
#define OWNCLOUDDBUSCONSTS_H

#define HARBOUR_OWNCLOUD_DAEMON_IFACE "com.github.beidl.HarbourOwncloud.Daemon.Uploader"
#define HARBOUR_OWNCLOUD_PERMD_IFACE  "com.github.beidl.HarbourOwncloud.Daemon.Permission"

#include <QString>
#include <QDBusArgument>

namespace HarbourOwncloud {
namespace DBusConsts {
// Photo backup daemon
const QString DBUS_SERVICE      = QStringLiteral(HARBOUR_OWNCLOUD_DAEMON_IFACE);
const QString DBUS_PATH         = QStringLiteral("/");
const QString DBUS_INTERFACE    = QStringLiteral(HARBOUR_OWNCLOUD_DAEMON_IFACE);

const QString DBUS_METHOD_RELOADCONFIG     = QStringLiteral("reloadConfig");
const QString DBUS_METHOD_ABORT            = QStringLiteral("abort");
const QString DBUS_SIGNAL_UPLOADINGCHANGED = QStringLiteral("uploadingChanged");
const QString DBUS_PROPERTY_UPLOADING      = QStringLiteral("uploading");

// User-session-wide permission dialog
namespace PermAgent {
const QString DBUS_SERVICE      = QStringLiteral(HARBOUR_OWNCLOUD_PERMD_IFACE);
const QString DBUS_PATH         = QStringLiteral("/");
const QString DBUS_INTERFACE    = QStringLiteral(HARBOUR_OWNCLOUD_PERMD_IFACE);

const QString DBUS_METHOD_REQUESTCONFIG = QStringLiteral("requestConfig");
}
}
}

#endif // OWNCLOUDDBUSCONSTS_H
