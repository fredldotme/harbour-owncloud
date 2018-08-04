#include "globaltransfermanager.h"

#include <settings/nextcloudsettings.h>

GlobalTransferManager* GlobalTransferManager::instance()
{
    NextcloudSettings* settings = NextcloudSettings::instance();
    if (settings)
        settings->readSettings();

    static GlobalTransferManager manager(Q_NULLPTR, settings);
    return &manager;
}

GlobalTransferManager::GlobalTransferManager(QObject *parent,
                                             NextcloudSettingsBase* settings) :
    WebDavCommandQueue(parent, settings)
{
}
