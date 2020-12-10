#include "ghostcloudshareplugin.h"
#include "ghostclouduploader.h"
#include "ghostcloudplugininfo.h"
#include "ghostcloudconsts.h"

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

