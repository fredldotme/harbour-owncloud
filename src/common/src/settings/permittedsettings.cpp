#include "permittedsettings.h"

#include <QDebug>
#include <QDBusInterface>
#include <QDBusReply>

#include <ownclouddbusconsts.h>

PermittedSettings::PermittedSettings(QObject *parent) :
    AccountBase(parent)
{
}

bool PermittedSettings::readSettings()
{
    QDBusInterface permissiondIface(HarbourOwncloud::DBusConsts::PermAgent::DBUS_SERVICE,
                                    HarbourOwncloud::DBusConsts::PermAgent::DBUS_PATH,
                                    HarbourOwncloud::DBusConsts::PermAgent::DBUS_INTERFACE,
                                    QDBusConnection::sessionBus());

    QDBusPendingCall pcall = permissiondIface.asyncCall(HarbourOwncloud::DBusConsts::PermAgent::DBUS_METHOD_REQUESTCONFIG);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [=](QDBusPendingCallWatcher* wtchr) {
        QDBusPendingReply<QMap<QString, QVariant> > reply = *wtchr;

        if (!reply.isError()) {
            QMap<QString, QVariant> values = reply.value();
            const bool isDenied = values.value(NEXTCLOUD_PERMD_REQUESTDENIED).toBool();

            if (!isDenied) {
                const QString md5Hex = values.value(NEXTCLOUD_SETTINGS_KEY_CERTMD5).toString();
                setMd5Hex(md5Hex);

                const QString sha1Hex = values.value(NEXTCLOUD_SETTINGS_KEY_CERTSHA1).toString();
                setSha1Hex(sha1Hex);

                const bool isHttps = values.value(NEXTCLOUD_SETTINGS_KEY_ISHTTPS).toBool();
                setIsHttps(isHttps);

                const QString hostname = values.value(NEXTCLOUD_SETTINGS_KEY_HOSTNAME).toString();
                setHostname(hostname);

                const int port = values.value(NEXTCLOUD_SETTINGS_KEY_PORT).toInt();
                setPort(port);

                const QString path = values.value(NEXTCLOUD_SETTINGS_KEY_PATH).toString();
                setPath(path);

                const QString username = values.value(NEXTCLOUD_SETTINGS_KEY_USERNAME).toString();
                setUsername(username);

                const QString password = values.value(NEXTCLOUD_SETTINGS_KEY_PASSWORD).toString();
                setPassword(password);

                const int providerType = values.value(NEXTCLOUD_SETTINGS_KEY_PROVIDERTYPE).toInt();
                setProviderType(providerType);

                refreshHostString();
            }

            // Since settings are read, propagate changes to the SettingsBasedCommandQueue object
            emit settingsChanged();
        } else {
            qWarning() << QDBusError::errorString(reply.error().type());
        }

        wtchr->deleteLater();
    });

    return true;
}
