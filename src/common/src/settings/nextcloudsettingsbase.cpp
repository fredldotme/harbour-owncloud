#include "nextcloudsettingsbase.h"

#include <QDebug>
#include <QStandardPaths>
#include <QUrl>

AccountBase::AccountBase(QObject *parent) : QObject(parent)
{
    QObject::connect(this, &AccountBase::customCertChanged,
                     this, [=](){
        qDebug() << "customCertChanged" << this->isCustomCert();
    });

    m_hoststring = "https://";
    m_isHttps = true;
    m_port = 443;
    m_autoLogin = false;
    m_notifications = true;
    m_providerType = ProviderType::Nextcloud;
    m_uploadAutomatically = false;
    m_mobileUpload = false;

    connect(this, &AccountBase::hoststringChanged, this, &AccountBase::settingsChanged);
    connect(this, &AccountBase::usernameChanged, this, &AccountBase::settingsChanged);
    connect(this, &AccountBase::passwordChanged, this, &AccountBase::settingsChanged);
    connect(this, &AccountBase::uploadAutomaticallyChanged, this, &AccountBase::settingsChanged);
    connect(this, &AccountBase::localPicturesPathChanged, this, &AccountBase::settingsChanged);
}

void AccountBase::refreshHostString()
{
    QString hostString;
    hostString = m_isHttps ? "https://" : "http://";
    hostString += m_hostname + ":" + QString::number(m_port) + m_path;

    if (hostString != m_hoststring) {
        m_hoststring = hostString;
        emit hoststringChanged();
    }
}

bool AccountBase::setHoststring(QString value)
{
    if(!value.startsWith("https://") && !value.startsWith("http://")) {
        return false;
    }

    QUrl url(value, QUrl::StrictMode);

    if(url.isValid()) {
        setIsHttps(value.startsWith("https://"));
        setHostname(url.host());
        setPath(url.path());
        if(path().isEmpty())
            setPath("/");
        if(!path().endsWith("/"))
            setPath(path() + "/");

        setPort(url.port());

        if(port() == -1) {
            setPort(isHttps() ? 443 : 80);
        }
    }
    refreshHostString();

    return url.isValid() && !url.host().isEmpty();
}

void AccountBase::resetSettings()
{
    setHostname("");
    setPath("/");
    setPort(443);
    setIsHttps(true);
    setUsername("");
    setPassword("");
    setProviderType(ProviderType::Nextcloud);
    setMd5Hex("");
    setSha1Hex("");

    refreshHostString();
    setAutoLogin(false);
    setUploadAutomatically(false);
    setMobileUpload(false);
    setNotifications(true);

    emit settingsChanged();
}

bool AccountBase::autoLogin() const
{
    return this->m_autoLogin;
}

void AccountBase::setAutoLogin(bool value)
{
    if (this->m_autoLogin == value)
        return;

    this->m_autoLogin = value;
    emit autoLoginChanged();
}

bool AccountBase::notifications() const
{
    return m_notifications;
}

void AccountBase::setNotifications(bool value)
{
    if (this->m_notifications == value)
        return;

    this->m_notifications = value;
    emit notificationSettingsChanged();
}

QString AccountBase::hostname() const
{
    return this->m_hostname;
}

void AccountBase::setHostname(const QString &value)
{
    if (this->m_hostname == value)
        return;

    this->m_hostname = value;
    Q_EMIT hostnameChanged();
}

QString AccountBase::path() const
{
    return this->m_path.isEmpty() ? "/" : this->m_path;
}

void AccountBase::setPath(const QString &value)
{
    if (this->m_path == value)
        return;

    this->m_path = value;
    Q_EMIT pathChanged();
}

int AccountBase::port() const
{
    return m_port;
}

void AccountBase::setPort(int value)
{
    if (this->m_port == value)
        return;

    this->m_port = value;
    Q_EMIT portChanged();
}

bool AccountBase::isHttps() const
{
    return m_isHttps;
}

void AccountBase::setIsHttps(bool value)
{
    if (this->m_isHttps == value)
        return;

    this->m_isHttps = value;
    Q_EMIT isHttpsChanged();
}

QString AccountBase::hoststring() const
{
    return m_hoststring;
}

QString AccountBase::username() const
{
    return m_username;
}

void AccountBase::setUsername(QString value)
{
    if (this->m_username == value)
        return;

    this->m_username = value;
    Q_EMIT usernameChanged();
}

QString AccountBase::password() const
{
    return m_password;
}

void AccountBase::setPassword(QString value)
{
    if (this->m_password == value)
        return;

    this->m_password = value;
    Q_EMIT passwordChanged();
}

int AccountBase::providerType() const
{
    return static_cast<int>(this->m_providerType);
}

void AccountBase::setProviderType(int type)
{
    const ProviderType providerType = static_cast<ProviderType>(type);
    if (this->m_providerType == providerType)
        return;

    qDebug() << "New value:" << providerType;
    this->m_providerType = providerType;
    Q_EMIT providerTypeChanged();
}

QString AccountBase::md5Hex() const
{
    return this->m_md5Hex;
}

void AccountBase::setMd5Hex(const QString &value)
{
    if (this->m_md5Hex == value)
        return;

    this->m_md5Hex = value;
    Q_EMIT md5HexChanged();
}

QString AccountBase::sha1Hex() const
{
    return this->m_sha1Hex;
}

void AccountBase::setSha1Hex(const QString &value)
{
    if (this->m_sha1Hex == value)
        return;

    this->m_sha1Hex = value;
    Q_EMIT sha1HexChanged();
}

bool AccountBase::uploadAutomatically() const
{
    return m_uploadAutomatically;
}

void AccountBase::setUploadAutomatically(bool enabled)
{
    qDebug() << Q_FUNC_INFO << enabled;
    if (this->m_uploadAutomatically == enabled)
        return;
    this->m_uploadAutomatically = enabled;
    Q_EMIT uploadAutomaticallyChanged();
}

bool AccountBase::mobileUpload() const
{
    return m_mobileUpload;
}

void AccountBase::setMobileUpload(bool enabled)
{
    if (this->m_mobileUpload == enabled)
        return;
    this->m_mobileUpload = enabled;
    Q_EMIT mobileUploadChanged();
}

void AccountBase::acceptTlsFingerprints(QString md5, QString sha1)
{
    if (this->m_md5Hex == md5 && this->m_sha1Hex == sha1)
        return;

    setMd5Hex(md5);
    setSha1Hex(sha1);

    Q_EMIT customCertChanged();
}

void AccountBase::setCustomCert(bool value)
{
    if (!value)
        acceptTlsFingerprints("", "");
}

bool AccountBase::isCustomCert() const
{
    return !(this->m_md5Hex.isEmpty() || this->m_sha1Hex.isEmpty());
}
