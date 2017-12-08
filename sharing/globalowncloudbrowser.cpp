#include "globalowncloudbrowser.h"
#include <nextcloudsettings.h>

GlobalOwncloudBrowser::GlobalOwncloudBrowser(QObject *parent, NextcloudSettingsBase* settings) :
    OwnCloudBrowser(parent, settings)
{

}

GlobalOwncloudBrowser* GlobalOwncloudBrowser::instance()
{
    NextcloudSettings::instance()->readSettings();
    static GlobalOwncloudBrowser browser(nullptr, NextcloudSettings::instance());
    return &browser;
}
