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

    Q_PROPERTY(QString hostname READ hostname)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(int port READ port)
    Q_PROPERTY(bool isHttps READ isHttps)

    Q_PROPERTY(QString hoststring READ hoststring NOTIFY hoststringChanged())
    Q_PROPERTY(QString username READ username WRITE setUsername(QString) NOTIFY usernameChanged())
    Q_PROPERTY(QString password READ password WRITE setPassword(QString))

    QString hostname();
    QString path();
    int port();
    bool isHttps();

    QString hoststring();
    QString username();
    QString password();

private:
    QSettings settings;

    QString m_hostname;
    QString m_path;
    int m_port;
    bool m_isHttps;

    QString m_hoststring;
    QString m_username;
    QString m_password;

    void setUsername(QString value);
    void setPassword(QString value);


signals:
    void settingsChanged();
    void usernameChanged();
    void hoststringChanged();

public slots:

};

#endif // SETTINGS_H
