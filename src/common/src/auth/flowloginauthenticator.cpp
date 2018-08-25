#include "flowloginauthenticator.h"

#include <QUrl>
#include <QDebug>

FlowLoginAuthenticator::FlowLoginAuthenticator(QObject *parent,
                                               NextcloudSettingsBase* settings) :
    Authenticator(parent, settings)
{
    QObject::connect(this, &FlowLoginAuthenticator::settingsChanged,
                     this, &FlowLoginAuthenticator::urlChanged);
}

void FlowLoginAuthenticator::authenticate()
{
    setRunning(true);
}

void FlowLoginAuthenticator::validateFlowResponse(QString responseUrl)
{
    setRunning(false);

    const QUrl ncUrl(responseUrl);

    if (ncUrl.scheme() != QStringLiteral("nc://")) {
        qWarning() << "Scheme" << ncUrl.scheme() << "is not valid";
        Q_EMIT authenticationFailed();
        return;
    }

    QStringList pairs = ncUrl.path().split(':');
    QString server, username, password;

    // Parse response url values
    for (QString pair : pairs) {
        if (pair.startsWith('&'))
            pair.mid(1);

        if (pair.startsWith(QStringLiteral("server")))
            server = pair.mid(pair.indexOf(':'));
        else if (pair.startsWith(QStringLiteral("user")))
            username = pair.mid(pair.indexOf(':'));
        else if (pair.startsWith(QStringLiteral("password")))
            password = pair.mid(pair.indexOf(':'));
    }

    if (server.isEmpty() || username.isEmpty() || password.isEmpty()) {
        qWarning() << "Received response" << responseUrl << "not valid";
        Q_EMIT authenticationFailed();
        return;
    }

    qDebug() << "nc:" << server << username;
    if (this->settings()) {
        this->settings()->setUsername(username);
        this->settings()->setPassword(password);
    }
    Q_EMIT authenticationSuccessful();
}
