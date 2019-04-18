#include <QtQml/QQmlExtensionPlugin>
#include <QtQml/QtQml>

#include <settings/permittedsettings.h>
#include <commandentity.h>
#include <provider/storage/webdavcommandqueue.h>
#include <qmlmap.h>
#include <util/filepathutil.h>

class HarbourOwncloudQmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("com.github.beidl.harbourowncloud"));
        qmlRegisterType<CommandEntity>(uri, 1, 0, "CommandEntity");
        qmlRegisterType<WebDavCommandQueue>(uri, 1, 0, "WebDavCommandQueue");
        qmlRegisterType<AccountBase>(uri, 1, 0, "NextcloudSettingsBase");
        qmlRegisterType<PermittedSettings>(uri, 1, 0, "PermittedSettings");
        qmlRegisterType<FilePathUtil>(uri, 1, 0, "FilePathUtil");
        qmlRegisterType<QmlMap>(uri, 1, 0, "QmlMap");
    }
};
