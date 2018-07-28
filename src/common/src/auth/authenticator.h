#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <QObject>

#include <settings/nextcloudsettingsbase.h>

class Authenticator : public QObject
{
    Q_OBJECT

    Q_PROPERTY(NextcloudSettingsBase* settings READ settings WRITE setSettings NOTIFY settingsChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)

public:
    explicit Authenticator(QObject *parent = Q_NULLPTR,
                           NextcloudSettingsBase* settings = Q_NULLPTR);

public slots:
    virtual void authenticate(bool saveCredentials = false) = 0;

protected:
    NextcloudSettingsBase* settings();
    void setSettings(NextcloudSettingsBase* v);
    bool running();
    void setRunning(bool v);

    bool m_saveCredentials = false;

private:
    NextcloudSettingsBase* m_settings = Q_NULLPTR;
    bool m_running = false;

signals:
    void authenticationFailed();
    void sslError(QString certMd5, QString certSha1);
    void authenticationSuccessful();
    void settingsChanged();
    void runningChanged();
};
Q_DECLARE_METATYPE(Authenticator*)

#endif // AUTHENTICATOR_H
