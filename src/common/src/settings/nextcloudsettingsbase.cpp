#include "nextcloudsettingsbase.h"

#include <QStandardPaths>
#include <QUrl>

NextcloudSettingsBase::NextcloudSettingsBase(QObject *parent) : QObject(parent)
{
    m_hoststring = "https://";
    m_isHttps = true;
    m_autoLogin = false;
    m_notifications = true;
    m_providerType = ProviderType::Nextcloud;

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
    m_providerType = ProviderType::Nextcloud;
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
    return this->m_autoLogin;
}

void NextcloudSettingsBase::setAutoLogin(bool value)
{
    if (this->m_autoLogin == value)
        return;

    this->m_autoLogin = value;
    emit autoLoginChanged();
}

bool NextcloudSettingsBase::notifications()
{
    return m_notifications;
}

void NextcloudSettingsBase::setNotifications(bool value)
{
    if (this->m_notifications == value)
        return;

    this->m_notifications = value;
    emit notificationSettingsChanged();
}

QString NextcloudSettingsBase::hostname()
{
    return this->m_hostname;
}

QString NextcloudSettingsBase::path()
{
    return this->m_path.isEmpty() ? "/" : this->m_path;
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
    if (this->m_username == value)
        return;

    this->m_username = value;
    Q_EMIT usernameChanged();
}

QString NextcloudSettingsBase::password()
{
    return m_password;
}

void NextcloudSettingsBase::setPassword(QString value)
{
    if (this->m_password == value)
        return;

    this->m_password = value;
    Q_EMIT passwordChanged();
}

int NextcloudSettingsBase::providerType()
{
    return this->m_providerType;
}

void NextcloudSettingsBase::setProviderType(int type)
{
    const ProviderType providerType = (ProviderType)type;
    if (this->m_providerType == providerType)
        return;

    this->m_providerType = providerType;
    Q_EMIT providerTypeChanged();
}

QString NextcloudSettingsBase::md5Hex()
{
    return this->m_md5Hex;
}

QString NextcloudSettingsBase::sha1Hex()
{
    return this->m_sha1Hex;
}

void NextcloudSettingsBase::acceptCertificate(QString md5, QString sha1)
{
    if (this->m_md5Hex == md5 && this->m_sha1Hex == sha1)
        return;

    this->m_md5Hex = md5;
    this->m_sha1Hex = sha1;
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
    return !(this->m_md5Hex.isEmpty() || this->m_sha1Hex.isEmpty());
}
