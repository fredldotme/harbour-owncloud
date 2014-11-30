#include "settings.h"

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    m_hoststring = "https://";
    m_isHttps = true;
}

bool Settings::parseFromAddressString(QString value)
{
    if(value.startsWith("https://")) {
        m_isHttps = true;
    } else if (value.startsWith("http://")) {
        m_isHttps = false;
    } else {
        return false;
    }

    QUrl url(value, QUrl::StrictMode);
    m_hostname = url.host();
    m_path = url.path();
    m_port = url.port();
    if(m_port == -1) {
        m_port = m_isHttps ? 443 : 80;
    }
    qDebug() << "hostname: " << m_hostname;
    qDebug() << "port: " << m_port;
    qDebug() << "path: " << m_path;

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
        m_port = settings.value("port").toInt();
        m_isHttps = settings.value("isHttps").toBool();

        m_hoststring = m_isHttps ? "https://" : "http://";
        m_hoststring += m_hostname + ":" + QString::number(m_port) + m_path;
        emit hoststringChanged();
    }


    if(settings.allKeys().contains("username"))
    {
        m_username = settings.value("username").toString();
        emit usernameChanged();
    }


    if(settings.allKeys().contains("password"))
    {
        m_password = QString(QByteArray::fromBase64(settings.value("password").toByteArray()));
        emit passwordChanged();
    }

    if(settings.allKeys().contains("certMD5") &&
            settings.allKeys().contains("certSHA1"))
    {
        m_md5Hex = settings.value("certMD5").toString();
        m_sha1Hex = settings.value("certSHA1").toString();
        if(isCustomCert()) {
            emit customCertChanged();
        }
    }

    settings.endGroup();
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
    settings.setValue("username", QVariant::fromValue<QString>(m_username));
    settings.setValue("password", QVariant::fromValue<QString>(m_password.toLatin1().toBase64()));
    settings.setValue("certMD5", QVariant::fromValue<QString>(m_md5Hex));
    settings.setValue("certSHA1", QVariant::fromValue<QString>(m_sha1Hex));
    settings.endGroup();

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
    acceptCertificate("", "");
}

QString Settings::hostname()
{
    return m_hostname;
}

QString Settings::path()
{
    return m_path;
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
