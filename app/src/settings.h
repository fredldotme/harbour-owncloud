#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <qwebdav.h>

class Settings : public QObject
{
    Q_OBJECT

public:
    Settings(QObject *parent = 0);

    Q_INVOKABLE bool parseFromAddressString(QString value);
    Q_INVOKABLE bool readSettings();
    Q_INVOKABLE void writeSettings();
    Q_INVOKABLE void acceptCertificate(QString md5, QString sha1);

    Q_PROPERTY(QString hostname READ hostname)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(int port READ port)
    Q_PROPERTY(bool isHttps READ isHttps)

    Q_PROPERTY(bool isCustomCert READ isCustomCert WRITE acceptCertificate(bool) NOTIFY customCertChanged())
    Q_PROPERTY(bool autoLogin READ isAutoLogin WRITE setAutoLogin(bool) NOTIFY autoLoginChanged())

    Q_PROPERTY(QString hoststring READ hoststring NOTIFY hoststringChanged())
    Q_PROPERTY(QString username READ username WRITE setUsername(QString) NOTIFY usernameChanged())
    Q_PROPERTY(QString password READ password WRITE setPassword(QString) NOTIFY passwordChanged())

    QString hostname();
    QString path();
    int port();
    bool isHttps();

    QString hoststring();
    QString username();
    QString password();

    bool isAutoLogin();

    QString md5Hex();
    QString sha1Hex();

private:
    QSettings settings;

    QString m_hostname;
    QString m_path;
    int m_port;
    bool m_isHttps;

    bool m_autoLogin;

    QString m_hoststring;
    QString m_username;
    QString m_password;

    QString m_md5Hex;
    QString m_sha1Hex;

    void setUsername(QString value);
    void setPassword(QString value);
    void setAutoLogin(bool value);
    void acceptCertificate(bool value);

    bool isCustomCert();

signals:
    void settingsChanged();
    void usernameChanged();
    void passwordChanged();
    void hoststringChanged();
    void autoLoginChanged();
    void customCertChanged();

public slots:

};

#endif // SETTINGS_H
