#include "nextcloudsettings.h"

#include <QStandardPaths>
#include <QDebug>

NextcloudSettings* NextcloudSettings::instance()
{
    static NextcloudSettings settings;
    return &settings;
}

NextcloudSettings::NextcloudSettings(QObject *parent) :
    NextcloudSettingsBase(parent),
    settings("harbour-owncloud", "harbour-owncloud")
{
}

bool NextcloudSettings::readSettings()
{
    settings.beginGroup("Settings");
    if(settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_HOSTNAME) &&
            settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_PATH) &&
            settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_PORT) &&
            settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_ISHTTPS))
    {
        m_hostname = settings.value(NEXTCLOUD_SETTINGS_KEY_HOSTNAME).toString();
        m_path = settings.value(NEXTCLOUD_SETTINGS_KEY_PATH).toString();
        if(m_path.isEmpty())
            m_path = "/";
        if(!m_path.endsWith("/"))
            m_path += "/";
        m_port = settings.value(NEXTCLOUD_SETTINGS_KEY_PORT).toInt();
        m_isHttps = settings.value(NEXTCLOUD_SETTINGS_KEY_ISHTTPS).toBool();

        refreshHostString();
    } else {
        m_hoststring = "https://";
        m_isHttps = true;
        emit hoststringChanged();
    }

    if(settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_AUTOLOGIN))
    {
        m_autoLogin = settings.value(NEXTCLOUD_SETTINGS_KEY_AUTOLOGIN).toBool();
    } else {
        m_autoLogin = false;
    }
    emit autoLoginChanged();

    if(settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS))
    {
        m_notifications = settings.value(NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS).toBool();
    } else {
        m_notifications = true;
    }
    emit notificationSettingsChanged();

    if(settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_USERNAME))
    {
        if (settings.value(NEXTCLOUD_SETTINGS_KEY_USERNAME).toString() != m_username) {
            m_username = settings.value(NEXTCLOUD_SETTINGS_KEY_USERNAME).toString();
            emit usernameChanged();
        }
    }

    if(settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_PASSWORD))
    {
        QString password = QString(QByteArray::fromBase64(settings.value(NEXTCLOUD_SETTINGS_KEY_PASSWORD).toByteArray()));

        if (password != m_password) {
            m_password = password;
            emit passwordChanged();
        }
    }

    if(settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_CERTMD5) &&
            settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_CERTSHA1))
    {
        QString md5Hex = settings.value(NEXTCLOUD_SETTINGS_KEY_CERTMD5).toString();
        QString sha1Hex = settings.value(NEXTCLOUD_SETTINGS_KEY_CERTSHA1).toString();
        if (md5Hex != m_md5Hex || sha1Hex != m_sha1Hex) {
            m_md5Hex = md5Hex;
            m_sha1Hex = sha1Hex;
            emit customCertChanged();
        }
    }

    m_uploadAutomatically = settings.value(NEXTCLOUD_SETTINGS_KEY_UPLOADAUTOMATICALLY,
                                           false).toBool();

    m_mobileUpload = settings.value(NEXTCLOUD_SETTINGS_KEY_MOBILEUPLOAD,
                                    false).toBool();

    m_localPicturesPath = settings.value(NEXTCLOUD_SETTINGS_KEY_PICTURESPATH,
                                         QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();

    settings.endGroup();

    // Since settings are read, propagate changes to the associated object
    return NextcloudSettingsBase::readSettings();
}

void NextcloudSettings::writeSettings()
{
    settings.beginGroup("Settings");
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_HOSTNAME, QVariant::fromValue<QString>(m_hostname));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_PATH, QVariant::fromValue<QString>(m_path));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_PORT, QVariant::fromValue<int>(m_port));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_ISHTTPS, QVariant::fromValue<bool>(m_isHttps));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_AUTOLOGIN, QVariant::fromValue<bool>(m_autoLogin));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS, QVariant::fromValue<bool>(m_notifications));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_USERNAME, QVariant::fromValue<QString>(m_username));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_PASSWORD, QVariant::fromValue<QString>(m_password.toLatin1().toBase64()));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_CERTMD5, QVariant::fromValue<QString>(m_md5Hex));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_CERTSHA1, QVariant::fromValue<QString>(m_sha1Hex));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_UPLOADAUTOMATICALLY, QVariant::fromValue<bool>(m_uploadAutomatically));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_MOBILEUPLOAD, QVariant::fromValue<bool>(m_mobileUpload));
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_PICTURESPATH, QVariant::fromValue<QString>(m_localPicturesPath));
    settings.endGroup();
    settings.sync();
}

void NextcloudSettings::resetSettings()
{
    settings.beginGroup("Settings");
    settings.clear();
    settings.endGroup();

    NextcloudSettingsBase::resetSettings();
}
