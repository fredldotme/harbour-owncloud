#include "globaltransfermanager.h"

GlobalTransferManager* GlobalTransferManager::instance(OwnCloudBrowser* browser)
{
    static GlobalTransferManager manager(NULL, browser);
    return &manager;
}

GlobalTransferManager::GlobalTransferManager(QObject *parent, OwnCloudBrowser *browser) :
    TransferManager(parent, browser)
{

}
