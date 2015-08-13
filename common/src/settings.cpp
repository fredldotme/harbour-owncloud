#include "settings.h"
#include <QStandardPaths>

Settings *Settings::instance()
{
    static Settings settings;
    return &settings;
}

Settings::Settings(QObject *parent) :
    QObject(parent),
    settings("harbour-owncloud", "harbour-owncloud")
{
    m_hoststring = "https://";
    m_isHttps = true;
    m_autoLogin = false;
    m_notifications = true;

    connect(this, SIGNAL(hoststringChanged()), SIGNAL(settingsChanged()));
    connect(this, SIGNAL(usernameChanged()), SIGNAL(settingsChanged()));
    connect(this, SIGNAL(passwordChanged()), SIGNAL(settingsChanged()));
    connect(this, SIGNAL(uploadAutomaticallyChanged()), SIGNAL(settingsChanged()));
    connect(this, SIGNAL(localPicturesPathChanged()), SIGNAL(settingsChanged()));
}

bool Settings::parseFromAddressString(QString value)
{
    if(!value.startsWith("https://") && !value.startsWith("http://")) {
        return false;
    }

    QUrl url(value, QUrl::StrictMode);
    if(url.isValid()) {
        if(url.host().isEmpty())
            return false;

        m_isHttps = value.startsWith("https://");
        m_hostname = url.host();
        m_path = url.path();
        if(m_path.isEmpty())
            m_path = "/";
        if(!m_path.endsWith("/"))
            m_path += "/";
        qDebug() << "PATH:" << m_path;
        m_port = url.port();

        if(m_port == -1) {
            m_port = m_isHttps ? 443 : 80;
        }
    }

    return url.isValid();
}

bool Settings::readSettings()
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

        QString hostString;
        hostString = m_isHttps ? "https://" : "http://";
        hostString += m_hostname + ":" + QString::number(m_port) + m_path;

        if (hostString != m_hoststring) {
            m_hoststring = hostString;
            emit hoststringChanged();
        }
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
    return true;
}

void Settings::writeSettings()
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

void Settings::resetSettings()
{
    settings.beginGroup("Settings");
    settings.clear();
    settings.endGroup();

    m_hostname = "";
    m_path = "/";
    m_port = 443;
    m_username = "";
    m_password = "";
    m_md5Hex = "";
    m_sha1Hex = "";

    m_hoststring = "https://";
    m_isHttps = true;
    m_autoLogin = false;
    m_uploadAutomatically = false;
    m_mobileUpload = false;
    m_notifications = true;

    emit hoststringChanged();
    emit usernameChanged();
    emit passwordChanged();

    emit settingsChanged();
}

void Settings::acceptCertificate(QString md5, QString sha1)
{
    m_md5Hex = md5;
    m_sha1Hex = sha1;
    writeSettings();
    emit customCertChanged();
}

void Settings::acceptCertificate(bool value)
{
    if(!value)
        acceptCertificate("", "");
}

bool Settings::isAutoLogin()
{
    return m_autoLogin;
}

void Settings::setAutoLogin(bool value)
{
    m_autoLogin = value;
    emit autoLoginChanged();
}

bool Settings::notifications()
{
    return m_notifications;
}

void Settings::setNotifications(bool value)
{
    m_notifications = value;
    emit notificationSettingsChanged();
}

QString Settings::hostname()
{
    return m_hostname;
}

QString Settings::path()
{
    return m_path.isEmpty() ? "/" : m_path;
}

int Settings::port()
{
    return m_port;
}

bool Settings::isHttps()
{
    return m_isHttps;
}

QString Settings::hoststring()
{
    return m_hoststring;
}

QString Settings::username()
{
    return m_username;
}

void Settings::setUsername(QString value)
{
    m_username = value;
}

QString Settings::password()
{
    return m_password;
}

void Settings::setPassword(QString value)
{
    m_password = value;
}

QString Settings::md5Hex()
{
    return m_md5Hex;
}

QString Settings::sha1Hex()
{
    return m_sha1Hex;
}

bool Settings::isCustomCert()
{
    return !m_md5Hex.isEmpty() && !m_sha1Hex.isEmpty();
}
