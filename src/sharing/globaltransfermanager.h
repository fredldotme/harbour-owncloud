#ifndef GLOBALTRANSFERMANAGER_H
#define GLOBALTRANSFERMANAGER_H

#include <webdavcommandqueue.h>

class GlobalTransferManager : public WebDavCommandQueue
{
    Q_OBJECT
public:
    static GlobalTransferManager* instance();

    explicit GlobalTransferManager(QObject *parent = Q_NULLPTR,
                                   NextcloudSettingsBase* settings = Q_NULLPTR);
};

#endif // GLOBALTRANSFERMANAGER_H
