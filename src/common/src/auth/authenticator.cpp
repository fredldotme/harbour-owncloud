#include "authenticator.h"

#include <QDebug>

Authenticator::Authenticator(QObject *parent, NextcloudSettingsBase* settings) :
    QObject(parent), m_settings(settings)
{
    QObject::connect(this, &Authenticator::authenticationSuccessful,
                     this, [=]() {
        if (this->m_saveCredentials && this->m_settings) {
            this->m_settings->writeSettings();
            qInfo() << "Credentials saved";
        }
    });
}

NextcloudSettingsBase* Authenticator::settings()
{
    return this->m_settings;
}

void Authenticator::setSettings(NextcloudSettingsBase *v)
{
    qDebug() << Q_FUNC_INFO << v;
    if (this->m_settings == v)
        return;

    if (this->m_settings)
        QObject::disconnect(this->m_settings, 0, 0, 0);

    this->m_settings = v;

    // Connect to changes of credentials, certificate and hostname settings
    if (this->m_settings) {
        QObject::connect(this->m_settings, &NextcloudSettingsBase::hoststringChanged,
                         this, &Authenticator::updateClientSettings);
        QObject::connect(this->m_settings, &NextcloudSettingsBase::usernameChanged,
                         this, &Authenticator::updateClientSettings);
        QObject::connect(this->m_settings, &NextcloudSettingsBase::passwordChanged,
                         this, &Authenticator::updateClientSettings);
        QObject::connect(this->m_settings, &NextcloudSettingsBase::customCertChanged,
                         this, &Authenticator::updateClientSettings);
    }

    Q_EMIT settingsChanged();
}

bool Authenticator::running()
{
    return this->m_running;
}

void Authenticator::setRunning(bool v)
{
    if (this->m_running == v)
        return;

    this->m_running = v;
    Q_EMIT runningChanged();
}
