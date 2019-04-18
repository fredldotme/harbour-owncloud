#include "authenticator.h"

#include <QDebug>

Authenticator::Authenticator(QObject *parent, AccountBase* settings) :
    QObject(parent), m_settings(settings)
{
}

AccountBase* Authenticator::settings()
{
    return this->m_settings;
}

void Authenticator::setSettings(AccountBase *v)
{
    qDebug() << Q_FUNC_INFO << v;
    if (this->m_settings == v)
        return;

    if (this->m_settings)
        QObject::disconnect(this->m_settings,
                            nullptr,
                            nullptr,
                            nullptr);

    this->m_settings = v;

    // Connect to changes of credentials, certificate and hostname settings
    if (this->m_settings) {
        QObject::connect(this->m_settings, &AccountBase::hoststringChanged,
                         this, &Authenticator::updateClientSettings);
        QObject::connect(this->m_settings, &AccountBase::usernameChanged,
                         this, &Authenticator::updateClientSettings);
        QObject::connect(this->m_settings, &AccountBase::passwordChanged,
                         this, &Authenticator::updateClientSettings);
        QObject::connect(this->m_settings, &AccountBase::providerTypeChanged,
                         this, &Authenticator::updateClientSettings);
        QObject::connect(this->m_settings, &AccountBase::customCertChanged,
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
