#include "nextcloudshareplugin.h"
#include "nextclouduploader.h"
#include "nextcloudplugininfo.h"
#include "nextcloudconsts.h"
#include <QtPlugin>

ExampleSharePlugin::ExampleSharePlugin()
{
}

ExampleSharePlugin::~ExampleSharePlugin()
{
}

MediaTransferInterface * ExampleSharePlugin::transferObject()
{
    return new ExampleUploader;
}

TransferPluginInfo *ExampleSharePlugin::infoObject()
{
    return new ExamplePluginInfo;
}

QString ExampleSharePlugin::pluginId() const
{
    return harourowncloud::consts::SHARE_ID;
}

bool ExampleSharePlugin::enabled() const
{
    return true;
}

