#ifndef GLOBALTRANSFERMANAGER_H
#define GLOBALTRANSFERMANAGER_H

#include <net/transfermanager.h>

class GlobalTransferManager : public TransferManager
{
    Q_OBJECT
public:
    static GlobalTransferManager* instance(OwnCloudBrowser* browser);
    explicit GlobalTransferManager(QObject *parent = 0, OwnCloudBrowser* browser = 0);
};

#endif // GLOBALTRANSFERMANAGER_H
