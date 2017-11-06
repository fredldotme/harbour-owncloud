#ifndef OWNCLOUDDBUSCONSTS_H
#define OWNCLOUDDBUSCONSTS_H

#define HARBOUR_OWNCLOUD_DAEMON_IFACE "com.github.beidl.HarbourOwncloud.Daemon.Uploader"

#include <QString>

namespace HarbourOwncloud {
namespace DBusConsts {
const QString DBUS_SERVICE = QStringLiteral("com.github.beidl.HarbourOwncloud.Daemon");
const QString DBUS_PATH = QStringLiteral("/");
const QString DBUS_INTERFACE = QStringLiteral(HARBOUR_OWNCLOUD_DAEMON_IFACE);

const QString DBUS_METHOD_RELOADCONFIG = QStringLiteral("reloadConfig");
const QString DBUS_SIGNAL_UPLOADINGCHANGED = QStringLiteral("uploadingChanged");
}
}

#endif // OWNCLOUDDBUSCONSTS_H
