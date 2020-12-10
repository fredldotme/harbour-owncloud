#include "ghostcloudshareplugin.h"
#include "ghostclouduploader.h"
#include "ghostcloudplugininfo.h"
#include "ghostcloudconsts.h"

#include <QtPlugin>

GhostCloudSharePlugin::GhostCloudSharePlugin()
{
}

GhostCloudSharePlugin::~GhostCloudSharePlugin()
{
}

MediaTransferInterface * GhostCloudSharePlugin::transferObject()
{
    return new GhostCloudUploader;
}

TransferPluginInfo *GhostCloudSharePlugin::infoObject()
{
    return new GhostCloudPluginInfo;
}

QString GhostCloudSharePlugin::pluginId() const
{
    return harourowncloud::consts::SHARE_ID;
}

bool GhostCloudSharePlugin::enabled() const
{
    return true;
}

