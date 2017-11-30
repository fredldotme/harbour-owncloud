#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <qwebdav.h>

class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isCustomCert READ isCustomCert WRITE acceptCertificate NOTIFY customCertChanged)
    Q_PROPERTY(bool autoLogin READ isAutoLogin WRITE setAutoLogin NOTIFY autoLoginChanged)
    Q_PROPERTY(bool notifications READ notifications WRITE setNotifications NOTIFY notificationSettingsChanged)

    Q_PROPERTY(QString hoststring READ hoststring NOTIFY hoststringChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

    Q_PROPERTY(bool uploadAutomatically READ uploadAutomatically WRITE setUploadAutomatically NOTIFY uploadAutomaticallyChanged)
    Q_PROPERTY(bool mobileUpload READ mobileUpload WRITE setMobileUpload NOTIFY mobileUploadChanged)
    Q_PROPERTY(QString localPicturesPath READ localPicturesPath WRITE setLocalPicturesPath NOTIFY localPicturesPathChanged)

public:
    static Settings *instance();

    QString hostname();
    QString path();
    int port();
    bool isHttps();

    QString hoststring();
    QString username();
    QString password();

    bool isAutoLogin();
    bool notifications();

    QString md5Hex();
    QString sha1Hex();

    bool uploadAutomatically() { return m_uploadAutomatically; }
    void setUploadAutomatically(bool enabled) { m_uploadAutomatically = enabled; emit uploadAutomaticallyChanged(); }
    bool mobileUpload() { return m_mobileUpload; }
    void setMobileUpload(bool enabled) { m_mobileUpload = enabled; emit mobileUploadChanged(); }
    QString localPicturesPath() { return m_localPicturesPath; }
    void setLocalPicturesPath(QString newPath) { m_localPicturesPath = newPath; emit localPicturesPathChanged(); }

    Q_INVOKABLE bool parseFromAddressString(QString value);
    Q_INVOKABLE void acceptCertificate(QString md5, QString sha1);
    Q_INVOKABLE void writeSettings();
    Q_INVOKABLE void resetSettings();
    Q_INVOKABLE bool readSettings();

    Q_PROPERTY(QString hostname READ hostname)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(int port READ port)
    Q_PROPERTY(bool isHttps READ isHttps)

private:
    Settings(QObject *parent = 0);

    QSettings settings;

    QString m_hostname;
    QString m_path;
    int m_port;
    bool m_isHttps;

    bool m_autoLogin;
    bool m_notifications;

    QString m_hoststring;
    QString m_username;
    QString m_password;

    QString m_md5Hex;
    QString m_sha1Hex;

    bool m_uploadAutomatically;
    bool m_mobileUpload;
    QString m_localPicturesPath;

    void setUsername(QString value);
    void setPassword(QString value);
    void setAutoLogin(bool value);
    void setNotifications(bool value);
    void acceptCertificate(bool value);

    bool isCustomCert();

signals:
    void settingsChanged();

    void usernameChanged();
    void passwordChanged();
    void hoststringChanged();
    void autoLoginChanged();
    void notificationSettingsChanged();
    void customCertChanged();
    void uploadAutomaticallyChanged();
    void mobileUploadChanged();
    void localPicturesPathChanged();
};

#endif // SETTINGS_H
