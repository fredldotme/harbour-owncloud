#ifndef AUTHENTICATIONEXAMINER_H
#define AUTHENTICATIONEXAMINER_H

#include <QObject>
#include <commands/http/httpgetcommandentity.h>
#include <settings/nextcloudsettingsbase.h>
#include <auth/authenticator.h>
#include <auth/qwebdavauthenticator.h>

class AuthenticationExaminer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool running READ running NOTIFY runningChanged)

public:
    explicit AuthenticationExaminer(QObject *parent = Q_NULLPTR);
    enum AuthenticationMethod {
        QWEBDAV = 0,
        FLOW_DIALOG
    };
    Q_ENUM(AuthenticationMethod)
    bool running();

public slots:
    bool examine(QString serverUrl, AuthenticationMethod method);

private:
    AuthenticationMethod m_method;
    NextcloudSettingsBase m_tempSettings;

    void setRunning(bool v);
    bool m_running = false;

signals:
    void examinationFailed(AuthenticationMethod method);
    void examinationSucceeded(AuthenticationMethod method);
    void runningChanged();
};
Q_DECLARE_METATYPE(AuthenticationExaminer*)

#endif // AUTHENTICATIONEXAMINER_H
