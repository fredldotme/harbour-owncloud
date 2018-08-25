#include "inifilesettings.h"

#include <QStandardPaths>
#include <QDebug>
#include <QFile>

IniFileSettings* IniFileSettings::instance()
{
    static IniFileSettings settings;
    return &settings;
}

IniFileSettings::IniFileSettings(QObject *parent) :
    NextcloudSettingsBase(parent),
    m_settings("harbour-owncloud", "harbour-owncloud")
{
}

bool IniFileSettings::readSettings()
{
    qDebug() << Q_FUNC_INFO;

    if (!QFile::exists(this->m_settings.fileName())) {
        qInfo() << ".ini-based settings file doesn't exist at path"
                << this->m_settings.fileName();
        return false;
    }

    this->m_settings.sync();
    this->m_settings.beginGroup("Settings");
    if (this->m_settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_HOSTNAME) &&
            this->m_settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_PATH) &&
            this->m_settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_PORT) &&
            this->m_settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_ISHTTPS))
    {
        const QString hostname = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_HOSTNAME).toString();
        setHostname(hostname);

        QString path = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_PATH).toString();
        if(path.isEmpty())
            path = "/";
        if(!path.endsWith("/"))
            path += "/";
        setPath(path);

        const int port = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_PORT).toInt();
        setPort(port);

        const bool isHttps = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_ISHTTPS).toBool();
        setIsHttps(isHttps);

        refreshHostString();
    } else {
        setHoststring("https://");
    }

    if (this->m_settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_AUTOLOGIN))
    {
        const bool autoLogin = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_AUTOLOGIN).toBool();
        setAutoLogin(autoLogin);
    } else {
        setAutoLogin(false);
    }

    if (this->m_settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS))
    {
        const bool notifications = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS).toBool();
        setNotifications(notifications);
    } else {
        setNotifications(true);
    }

    if (this->m_settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_USERNAME))
    {
        const QString username = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_USERNAME).toString();
        setUsername(username);
    }

    if (this->m_settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_PASSWORD))
    {
        const QString password = QString(QByteArray::fromBase64(this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_PASSWORD).toByteArray()));
        setPassword(password);
    }

    if (this->m_settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_PROVIDERTYPE))
    {
        const ProviderType providerType = (ProviderType)this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_PROVIDERTYPE).toInt();
        setProviderType(providerType);
    }

    if (this->m_settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_CERTMD5) &&
            this->m_settings.allKeys().contains(NEXTCLOUD_SETTINGS_KEY_CERTSHA1))
    {
        QString md5Hex = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_CERTMD5).toString();
        QString sha1Hex = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_CERTSHA1).toString();
        acceptCertificate(md5Hex, sha1Hex);
    }

    const bool uploadAutomatically = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_UPLOADAUTOMATICALLY,
                                                            false).toBool();
    setUploadAutomatically(uploadAutomatically);

    const bool mobileUpload = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_MOBILEUPLOAD,
                                                     false).toBool();
    setMobileUpload(mobileUpload);

    const QString localPicturesPath = this->m_settings.value(NEXTCLOUD_SETTINGS_KEY_PICTURESPATH,
                                                             QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
    setLocalPicturesPath(localPicturesPath);

    this->m_settings.endGroup();

    Q_EMIT settingsChanged();
    return true;
}

QString IniFileSettings::filePath()
{
    return this->m_settings.fileName();
}
