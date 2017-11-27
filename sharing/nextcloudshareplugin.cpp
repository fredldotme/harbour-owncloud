#include "nextcloudshareplugin.h"
#include "nextclouduploader.h"
#include "nextcloudplugininfo.h"
#include "nextcloudconsts.h"
#include <QtPlugin>

NextcloudSharePlugin::NextcloudSharePlugin()
{
}

NextcloudSharePlugin::~NextcloudSharePlugin()
{
}

MediaTransferInterface * NextcloudSharePlugin::transferObject()
{
    return new NextcloudUploader;
}

TransferPluginInfo *NextcloudSharePlugin::infoObject()
{
    return new NextcloudPluginInfo;
}

QString NextcloudSharePlugin::pluginId() const
{
    return harourowncloud::consts::SHARE_ID;
}

bool NextcloudSharePlugin::enabled() const
{
    return true;
}

