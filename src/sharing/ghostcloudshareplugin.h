#ifndef NEXTCLOUDSHAREPLUGIN_H
#define NEXTCLOUDSHAREPLUGIN_H

#include <TransferEngine-qt5/transferplugininterface.h>
#include "ghostcloudconsts.h"
#include <QObject>

class Q_DECL_EXPORT GhostCloudSharePlugin : public QObject, public TransferPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.github.beidl.harbour-owncloud.sharing")
    Q_INTERFACES(TransferPluginInterface)
public:
    GhostCloudSharePlugin();
    ~GhostCloudSharePlugin();

    MediaTransferInterface * transferObject();
    TransferPluginInfo *infoObject();
    QString pluginId() const;
    bool enabled() const ;

};

#endif // NEXTCLOUDSHAREPLUGIN_H
