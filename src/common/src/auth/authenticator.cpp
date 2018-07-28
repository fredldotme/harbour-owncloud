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
    if (this->m_settings == v)
        return;

    this->m_settings = v;
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
