#include "nextcloudplugininfo.h"
#include "nextcloudconsts.h"

#include <QUrl>

NextcloudPluginInfo::NextcloudPluginInfo()
    : m_ready(false)
{
}

NextcloudPluginInfo::~NextcloudPluginInfo()
{
}

QList<TransferMethodInfo> NextcloudPluginInfo::info() const
{
    return m_infoList;
}

void NextcloudPluginInfo::query()
{
    TransferMethodInfo info;
    QStringList capabilities;

    // Capabilites ie. what mimetypes this plugin supports
    capabilities << QLatin1String("image/*")
                 << QLatin1String("video/*")
                 << QLatin1String("text/*")
                 << QLatin1String("application/*");

    // TODO: Translations for 3rd party plugins is not yet supported by Sailfish OS.
    //       Adding support there later, but for now just use what ever non-translated
    //       string here. This string will be visible in the share method list.
    //: Display name for example share plugin
    //% "Example plugin"
    info.displayName     = QStringLiteral("GhostCloud");

    // Method ID is a unique identifier for this plugin. It is used to identify which share plugin should be
    // used for starting the sharing.
    info.methodId        = harourowncloud::consts::SHARE_ID;

    // Path to the Sharing UI which this plugin provides.
    info.shareUIPath     = QStringLiteral("/usr/share/nemo-transferengine/plugins/NextcloudShareUI.qml");

    info.accountIcon     = QStringLiteral("image://theme/icon-m-share-harbour-owncloud");

    // Pass information about capabilities. This info is used for filtering share plugins
    // which don't support defined types. For example, this plugin won't appear in the
    // share method list, if someone tries to share content which isn't image or vcard type,
    info.capabilitities  = capabilities;

    m_infoList << info;

    // Let the world know that this plugin is ready
    m_ready = true;
    emit infoReady();
}


bool NextcloudPluginInfo::ready() const
{
    return m_ready;
}
