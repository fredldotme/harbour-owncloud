#include "nextcloudsettingsbase.h"

#include <QStandardPaths>
#include <QUrl>

NextcloudSettingsBase::NextcloudSettingsBase(QObject *parent) : QObject(parent)
{
    m_hoststring = "https://";
    m_isHttps = true;
    m_autoLogin = false;
    m_notifications = true;

//    connect(this, &NextcloudSettingsBase::hoststringChanged, this, &NextcloudSettingsBase::settingsChanged);
//    connect(this, &NextcloudSettingsBase::usernameChanged, this, &NextcloudSettingsBase::settingsChanged);
//    connect(this, &NextcloudSettingsBase::passwordChanged, this, &NextcloudSettingsBase::settingsChanged);
//    connect(this, &NextcloudSettingsBase::uploadAutomaticallyChanged, this, &NextcloudSettingsBase::settingsChanged);
//    connect(this, &NextcloudSettingsBase::localPicturesPathChanged, this, &NextcloudSettingsBase::settingsChanged);
}

void NextcloudSettingsBase::refreshHostString()
{
    QString hostString;
    hostString = m_isHttps ? "https://" : "http://";
    hostString += m_hostname + ":" + QString::number(m_port) + m_path;

    if (hostString != m_hoststring) {
        m_hoststring = hostString;
        emit hoststringChanged();
    }
}

bool NextcloudSettingsBase::parseFromAddressString(QString value)
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

        m_port = url.port();

        if(m_port == -1) {
            m_port = m_isHttps ? 443 : 80;
        }
    }

    return url.isValid();
}

bool NextcloudSettingsBase::readSettings()
{
    Q_EMIT settingsChanged();
    return true;
}

void NextcloudSettingsBase::writeSettings()
{
}

void NextcloudSettingsBase::resetSettings()
{
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

bool NextcloudSettingsBase::isAutoLogin()
{
    return m_autoLogin;
}

void NextcloudSettingsBase::setAutoLogin(bool value)
{
    m_autoLogin = value;
    emit autoLoginChanged();
}

bool NextcloudSettingsBase::notifications()
{
    return m_notifications;
}

void NextcloudSettingsBase::setNotifications(bool value)
{
    m_notifications = value;
    emit notificationSettingsChanged();
}

QString NextcloudSettingsBase::hostname()
{
    return m_hostname;
}

QString NextcloudSettingsBase::path()
{
    return m_path.isEmpty() ? "/" : m_path;
}

int NextcloudSettingsBase::port()
{
    return m_port;
}

bool NextcloudSettingsBase::isHttps()
{
    return m_isHttps;
}

QString NextcloudSettingsBase::hoststring()
{
    return m_hoststring;
}

QString NextcloudSettingsBase::username()
{
    return m_username;
}

void NextcloudSettingsBase::setUsername(QString value)
{
    m_username = value;
}

QString NextcloudSettingsBase::password()
{
    return m_password;
}

void NextcloudSettingsBase::setPassword(QString value)
{
    m_password = value;
}

QString NextcloudSettingsBase::md5Hex()
{
    return m_md5Hex;
}

QString NextcloudSettingsBase::sha1Hex()
{
    return m_sha1Hex;
}

void NextcloudSettingsBase::acceptCertificate(QString md5, QString sha1)
{
    m_md5Hex = md5;
    m_sha1Hex = sha1;
    writeSettings();
    emit customCertChanged();
}

void NextcloudSettingsBase::acceptCertificate(bool value)
{
    if(!value)
        acceptCertificate("", "");
}

bool NextcloudSettingsBase::isCustomCert()
{
    return !m_md5Hex.isEmpty() && !m_sha1Hex.isEmpty();
}
