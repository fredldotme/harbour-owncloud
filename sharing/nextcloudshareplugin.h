#ifndef NEXTCLOUDSHAREPLUGIN_H
#define NEXTCLOUDSHAREPLUGIN_H

#include <TransferEngine-qt5/transferplugininterface.h>
#include "nextcloudconsts.h"
#include <QObject>

class Q_DECL_EXPORT NextcloudSharePlugin : public QObject, public TransferPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID HARBOUROWNCLOUD_PLUGINID)
    Q_INTERFACES(TransferPluginInterface)
public:
    NextcloudSharePlugin();
    ~NextcloudSharePlugin();

    MediaTransferInterface * transferObject();
    TransferPluginInfo *infoObject();
    QString pluginId() const;
    bool enabled() const ;

};

#endif // NEXTCLOUDSHAREPLUGIN_H
