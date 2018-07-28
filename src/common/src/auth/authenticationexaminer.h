#ifndef AUTHENTICATIONEXAMINER_H
#define AUTHENTICATIONEXAMINER_H

#include <QObject>
#include <commands/http/httpgetcommandentity.h>
#include <settings/examinationsettings.h>
#include <auth/authenticator.h>
#include <auth/qwebdavauthenticator.h>

class AuthenticationExaminer : public QObject
{
    Q_OBJECT

public:
    explicit AuthenticationExaminer(QObject *parent = Q_NULLPTR);
    enum AuthenticationMethod {
        QWEBDAV = 0,
        FLOW_DIALOG
    };
    Q_ENUM(AuthenticationMethod)

public slots:
    bool examine(QString serverUrl, AuthenticationMethod method);

private:
    AuthenticationMethod m_method;
    ExaminationSettings m_tempSettings;

    const QVector<Authenticator*> supportedAuthenticators {
        new QWebDavAuthenticator
    };

signals:
    void examinationFailed(AuthenticationMethod method);
    void examinationSucceeded(AuthenticationMethod method);
};
Q_DECLARE_METATYPE(AuthenticationExaminer*)

#endif // AUTHENTICATIONEXAMINER_H
