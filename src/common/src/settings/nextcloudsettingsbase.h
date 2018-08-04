#ifndef NEXTCLOUDSETTINGSBASE_H
#define NEXTCLOUDSETTINGSBASE_H

#include <QObject>
#include <QString>

const QString NEXTCLOUD_SETTINGS_KEY_CERTMD5 = QStringLiteral("certMD5");
const QString NEXTCLOUD_SETTINGS_KEY_CERTSHA1 = QStringLiteral("certSHA1");
const QString NEXTCLOUD_SETTINGS_KEY_ISHTTPS = QStringLiteral("isHttps");
const QString NEXTCLOUD_SETTINGS_KEY_HOSTNAME = QStringLiteral("hostname");
const QString NEXTCLOUD_SETTINGS_KEY_PORT = QStringLiteral("port");
const QString NEXTCLOUD_SETTINGS_KEY_PATH = QStringLiteral("path");
const QString NEXTCLOUD_SETTINGS_KEY_USERNAME = QStringLiteral("username");
const QString NEXTCLOUD_SETTINGS_KEY_PASSWORD = QStringLiteral("password");
const QString NEXTCLOUD_SETTINGS_KEY_PROVIDERTYPE = QStringLiteral("providerType");
const QString NEXTCLOUD_SETTINGS_KEY_PICTURESPATH = QStringLiteral("localPicturesPath");
const QString NEXTCLOUD_SETTINGS_KEY_MOBILEUPLOAD = QStringLiteral("mobileUpload");
const QString NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS = QStringLiteral("notifications");
const QString NEXTCLOUD_SETTINGS_KEY_UPLOADAUTOMATICALLY = QStringLiteral("uploadAutomatically");
const QString NEXTCLOUD_SETTINGS_KEY_AUTOLOGIN = QStringLiteral("autoLogin");
const QString NEXTCLOUD_PERMD_REQUESTDENIED = QStringLiteral("requestDenied");

class NextcloudSettingsBase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool autoLogin READ isAutoLogin WRITE setAutoLogin NOTIFY autoLoginChanged)
    Q_PROPERTY(bool notifications READ notifications WRITE setNotifications NOTIFY notificationSettingsChanged)

    Q_PROPERTY(QString hoststring READ hoststring NOTIFY hoststringChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool isCustomCert READ isCustomCert WRITE acceptCertificate NOTIFY customCertChanged)

    Q_PROPERTY(int providerType READ providerType WRITE setProviderType NOTIFY providerTypeChanged)

    Q_PROPERTY(bool uploadAutomatically READ uploadAutomatically WRITE setUploadAutomatically NOTIFY uploadAutomaticallyChanged)
    Q_PROPERTY(bool mobileUpload READ mobileUpload WRITE setMobileUpload NOTIFY mobileUploadChanged)
    Q_PROPERTY(QString localPicturesPath READ localPicturesPath WRITE setLocalPicturesPath NOTIFY localPicturesPathChanged)

    Q_PROPERTY(QString hostname READ hostname)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(int port READ port)
    Q_PROPERTY(bool isHttps READ isHttps)

public:
    NextcloudSettingsBase(QObject *parent = 0);

    enum ProviderType {
        Nextcloud = 0,
        WebDav
    };
    Q_ENUMS(ProviderType)

    QString hostname();
    QString path();
    int port();
    bool isHttps();

    QString hoststring();
    QString username();
    QString password();
    int providerType();
    bool isCustomCert();

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

public slots:
    void resetSettings();
    virtual bool readSettings();
    virtual void writeSettings();

    void setUsername(QString value);
    void setPassword(QString value);
    void setProviderType(int type);
    void acceptCertificate(QString md5, QString sha1);
    void acceptCertificate(bool value);

protected:
    void refreshHostString();

    QString m_hostname;
    QString m_path;
    int m_port;
    bool m_isHttps;
    ProviderType m_providerType;

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

private:
    void setAutoLogin(bool value);
    void setNotifications(bool value);

signals:
    void settingsChanged();

    void usernameChanged();
    void passwordChanged();
    void providerTypeChanged();
    void hoststringChanged();
    void customCertChanged();
    void autoLoginChanged();
    void notificationSettingsChanged();
    void uploadAutomaticallyChanged();
    void mobileUploadChanged();
    void localPicturesPathChanged();
};
Q_DECLARE_METATYPE(NextcloudSettingsBase*)

#endif // NEXTCLOUDSETTINGSBASE_H
