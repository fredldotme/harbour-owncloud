#include "cloudstorageprovider.h"

CloudStorageProvider::CloudStorageProvider(QObject *parent, NextcloudSettingsBase* settings) :
    CommandQueue(parent), m_settings(settings)
{
    setSettings(settings);
}

NextcloudSettingsBase* CloudStorageProvider::settings()
{
    return this->m_settings;
}

void CloudStorageProvider::setSettings(NextcloudSettingsBase *v)
{
    if (this->m_settings == v)
        return;

    // Disconnect from current settings object if it exists
    if (this->m_settings)
        QObject::disconnect(this->m_settings, 0, 0, 0);

    this->m_settings = v;
    Q_EMIT settingsChanged();
}
