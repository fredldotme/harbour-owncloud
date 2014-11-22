#include "settings.h"

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    m_hoststring = "https://";
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

    writeSettings();
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
    } else {
        settings.endGroup();
        return false;
    }
    m_hoststring = m_isHttps ? "https://" : "http://";
    m_hoststring += m_hostname + ":" + QString::number(m_port) + "/" + m_path;
    emit hoststringChanged();

    if(settings.allKeys().contains("username"))
    {
        m_username = settings.value("username").toString();
        settings.endGroup();
    } else {
        settings.endGroup();
        return false;
    }
    emit usernameChanged();

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
    settings.endGroup();

    emit settingsChanged();
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
