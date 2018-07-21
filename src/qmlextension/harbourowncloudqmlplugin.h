#include <QtQml/QQmlExtensionPlugin>
#include <QtQml/QtQml>

#include <net/owncloudbrowser.h>
#include <settings/permittedsettings.h>

class HarbourOwncloudQmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("com.github.beidl.harbourowncloud"));
        qmlRegisterType<OwnCloudBrowser>(uri, 1, 0, "OwnCloudBrowser");
        qmlRegisterType<NextcloudSettingsBase>(uri, 1, 0, "NextcloudSettingsBase");
        qmlRegisterType<PermittedSettings>(uri, 1, 0, "PermittedSettings");
    }
};
