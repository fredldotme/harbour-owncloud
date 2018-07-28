#include <QtQml/QQmlExtensionPlugin>
#include <QtQml/QtQml>

#include <net/owncloudbrowser.h>
#include <settings/permittedsettings.h>
#include <webdavcommandqueue.h>

class HarbourOwncloudQmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("com.github.beidl.harbourowncloud"));
        qmlRegisterType<WebDavCommandQueue>(uri, 1, 0, "WebDavCommandQueue");
        qmlRegisterType<NextcloudSettingsBase>(uri, 1, 0, "NextcloudSettingsBase");
        qmlRegisterType<PermittedSettings>(uri, 1, 0, "PermittedSettings");
    }
};
