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
        const bool autoLogin = settings.value(NEXTCLOUD_SETTINGS_KEY_AUTOLOGIN).toBool();
        setAutoLogin(autoLogin);
    } else {
        setAutoLogin(false);
    }

    if(settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS))
    {
        const bool notifications = settings.value(NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS).toBool();
        setNotifications(notifications);
    } else {
        setNotifications(true);
    }

    if(settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_USERNAME))
    {
        const QString username = settings.value(NEXTCLOUD_SETTINGS_KEY_USERNAME).toString();
        setUsername(username);
    }

    if(settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_PASSWORD))
    {
        const QString password = QString(QByteArray::fromBase64(settings.value(NEXTCLOUD_SETTINGS_KEY_PASSWORD).toByteArray()));
        setPassword(password);
    }

    if(settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_PROVIDERTYPE))
    {
        ProviderType providerType = (ProviderType)settings.value(NEXTCLOUD_SETTINGS_KEY_PROVIDERTYPE).toInt();
        setProviderType(providerType);
    }

    if(settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_CERTMD5) &&
            settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_CERTSHA1))
    {
        QString md5Hex = settings.value(NEXTCLOUD_SETTINGS_KEY_CERTMD5).toString();
        QString sha1Hex = settings.value(NEXTCLOUD_SETTINGS_KEY_CERTSHA1).toString();
        acceptCertificate(md5Hex, sha1Hex, false);
    }

    const bool uploadAutomatically = settings.value(NEXTCLOUD_SETTINGS_KEY_UPLOADAUTOMATICALLY,
                                                    false).toBool();
    setUploadAutomatically(uploadAutomatically);

    const bool mobileUpload = settings.value(NEXTCLOUD_SETTINGS_KEY_MOBILEUPLOAD,
                                             false).toBool();
    setMobileUpload(mobileUpload);

    const QString localPicturesPath = settings.value(NEXTCLOUD_SETTINGS_KEY_PICTURESPATH,
                                                     QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
    setLocalPicturesPath(localPicturesPath);

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
    settings.setValue(NEXTCLOUD_SETTINGS_KEY_PROVIDERTYPE, QVariant::fromValue<int>(m_providerType));
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
