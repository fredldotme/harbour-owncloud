#include <QtQml/QQmlExtensionPlugin>
#include <QtQml/QtQml>

#include <owncloudbrowser.h>

class HarbourOwncloudQmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("com.github.beidl.harbourowncloud"));
        qmlRegisterType<OwnCloudBrowser>(uri, 1, 0, "OwnCloudBrowser");
    }
};
