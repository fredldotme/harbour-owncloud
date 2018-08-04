#include "abstractfetcher.h"

#include <nextcloudendpointconsts.h>
#include <commands/http/httpgetcommandentity.h>

#include <QStandardPaths>

AbstractFetcher::AbstractFetcher(QObject *parent) : QObject(parent)
{
}


bool AbstractFetcher::fetching()
{
    return this->m_fetching;
}

void AbstractFetcher::setFetching(bool v)
{
    if (this->m_fetching == v)
        return;

    this->m_fetching = v;
    Q_EMIT fetchingChanged();
}

QString AbstractFetcher::source()
{
    return this->m_source;
}

void AbstractFetcher::setSource(const QString &v)
{
    if (this->m_source == v)
        return;

    this->m_source = v;
    qDebug() << "new thumbnail source:" << this->m_source;
    Q_EMIT sourceChanged();
}

NextcloudSettingsBase* AbstractFetcher::settings()
{
    return this->m_settings;
}

void AbstractFetcher::setSettings(NextcloudSettingsBase* v)
{
    if (this->m_settings == v)
        return;

    this->m_settings = v;
    Q_EMIT settingsChanged();
}

CacheProvider* AbstractFetcher::cacheProvider()
{
    return this->m_cacheProvider;
}

void AbstractFetcher::setCacheProvider(CacheProvider *v)
{
    if (this->m_cacheProvider == v)
        return;

    this->m_cacheProvider = v;
    Q_EMIT cacheProviderChanged();
}

CommandQueue* AbstractFetcher::commandQueue()
{
    return this->m_commandQueue;
}

void AbstractFetcher::setCommandQueue(CommandQueue* v)
{
    if (this->m_commandQueue == v)
        return;

    this->m_commandQueue = v;
    Q_EMIT settingsChanged();
}

int AbstractFetcher::width()
{
    return this->m_width;
}

void AbstractFetcher::setWidth(int v)
{
    if (this->m_width == v)
        return;

    this->m_width = v;
    Q_EMIT widthChanged();
}

int AbstractFetcher::height()
{
    return this->m_height;
}

void AbstractFetcher::setHeight(int v)
{
    if (this->m_height == v)
        return;

    this->m_height = v;
    Q_EMIT heightChanged();
}
