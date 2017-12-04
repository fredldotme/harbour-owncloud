#include "nextcloudsettings.h"

#include <QStandardPaths>

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
    if(settings.allKeys().contains("hostname") &&
            settings.allKeys().contains("path") &&
            settings.allKeys().contains("port") &&
            settings.allKeys().contains("isHttps"))
    {
        m_hostname = settings.value("hostname").toString();
        m_path = settings.value("path").toString();
        if(m_path.isEmpty())
            m_path = "/";
        if(!m_path.endsWith("/"))
            m_path += "/";
        m_port = settings.value("port").toInt();
        m_isHttps = settings.value("isHttps").toBool();

        refreshHostString();
    } else {
        m_hoststring = "https://";
        m_isHttps = true;
        emit hoststringChanged();
    }

    if(settings.allKeys().contains("autoLogin"))
    {
        m_autoLogin = settings.value("autoLogin").toBool();
    } else {
        m_autoLogin = false;
    }
    emit autoLoginChanged();

    if(settings.allKeys().contains("notifications"))
    {
        m_notifications = settings.value("notifications").toBool();
    } else {
        m_notifications = true;
    }
    emit notificationSettingsChanged();

    if(settings.allKeys().contains("username"))
    {
        if (settings.value("username").toString() != m_username) {
            m_username = settings.value("username").toString();
            emit usernameChanged();
        }
    }

    if(settings.allKeys().contains("password"))
    {
        QString password = QString(QByteArray::fromBase64(settings.value("password").toByteArray()));

        if (password != m_password) {
            m_password = password;
            emit passwordChanged();
        }
    }

    if(settings.allKeys().contains("certMD5") &&
            settings.allKeys().contains("certSHA1"))
    {
        QString md5Hex = settings.value("certMD5").toString();
        QString sha1Hex = settings.value("certSHA1").toString();
        if (md5Hex != m_md5Hex && sha1Hex != m_sha1Hex) {
            m_md5Hex = md5Hex;
            m_sha1Hex = sha1Hex;
            emit customCertChanged();
        }
    }

    m_uploadAutomatically = settings.value("uploadAutomatically", false).toBool();
    m_mobileUpload = settings.value("mobileUpload", false).toBool();
    m_localPicturesPath = settings.value("localPicturesPath", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();

    settings.endGroup();

    // Since settings are read, propagate changes to the ownCloud browser object
    emit settingsChanged();

    return NextcloudSettingsBase::readSettings();
}

void NextcloudSettings::writeSettings()
{
    settings.beginGroup("Settings");
    settings.setValue("hostname", QVariant::fromValue<QString>(m_hostname));
    settings.setValue("path", QVariant::fromValue<QString>(m_path));
    settings.setValue("port", QVariant::fromValue<int>(m_port));
    settings.setValue("isHttps", QVariant::fromValue<bool>(m_isHttps));
    settings.setValue("autoLogin", QVariant::fromValue<bool>(m_autoLogin));
    settings.setValue("notifications", QVariant::fromValue<bool>(m_notifications));
    settings.setValue("username", QVariant::fromValue<QString>(m_username));
    settings.setValue("password", QVariant::fromValue<QString>(m_password.toLatin1().toBase64()));
    settings.setValue("certMD5", QVariant::fromValue<QString>(m_md5Hex));
    settings.setValue("certSHA1", QVariant::fromValue<QString>(m_sha1Hex));
    settings.setValue("uploadAutomatically", QVariant::fromValue<bool>(m_uploadAutomatically));
    settings.setValue("mobileUpload", QVariant::fromValue<bool>(m_mobileUpload));
    settings.setValue("localPicturesPath", QVariant::fromValue<QString>(m_localPicturesPath));
    settings.endGroup();
}

void NextcloudSettings::resetSettings()
{
    settings.beginGroup("Settings");
    settings.clear();
    settings.endGroup();

    NextcloudSettingsBase::resetSettings();
}

void NextcloudSettings::acceptCertificate(QString md5, QString sha1)
{
    m_md5Hex = md5;
    m_sha1Hex = sha1;
    writeSettings();
    emit customCertChanged();
}

void NextcloudSettings::acceptCertificate(bool value)
{
    if(!value)
        acceptCertificate("", "");
}

bool NextcloudSettings::isCustomCert()
{
    return !m_md5Hex.isEmpty() && !m_sha1Hex.isEmpty();
}
