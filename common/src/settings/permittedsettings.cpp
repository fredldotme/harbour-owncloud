#include "permittedsettings.h"

#include <QDebug>
#include <QDBusInterface>
#include <QDBusReply>

#include <ownclouddbusconsts.h>

PermittedSettings::PermittedSettings(QObject *parent) :
    NextcloudSettingsBase(parent)
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
                m_md5Hex = values.value(NEXTCLOUD_SETTINGS_KEY_CERTMD5).toString();
                m_sha1Hex = values.value(NEXTCLOUD_SETTINGS_KEY_CERTSHA1).toString();
                m_isHttps = values.value(NEXTCLOUD_SETTINGS_KEY_ISHTTPS).toBool();
                m_hostname = values.value(NEXTCLOUD_SETTINGS_KEY_HOSTNAME).toString();
                m_port = values.value(NEXTCLOUD_SETTINGS_KEY_PORT).toInt();
                m_path = values.value(NEXTCLOUD_SETTINGS_KEY_PATH).toString();
                m_username = values.value(NEXTCLOUD_SETTINGS_KEY_USERNAME).toString();
                m_password = values.value(NEXTCLOUD_SETTINGS_KEY_PASSWORD).toString();
                refreshHostString();
            }

            // Since settings are read, propagate changes to the ownCloud browser object
            emit settingsChanged();
        } else {
            qWarning() << QDBusError::errorString(reply.error().type());
        }

        wtchr->deleteLater();
    });

    return NextcloudSettingsBase::readSettings();
}
