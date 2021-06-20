#include "sharedstatecontroller.h"

#include <commands/sync/ncdirtreecommandunit.h>

SharedStateController::SharedStateController(QObject *parent) :
    QObject(parent)
{
}

bool SharedStateController::requestDirectoryListing(AccountBase* account, QString subPath)
{
    if (!this->generator()) {
        qWarning() << Q_FUNC_INFO << "Generator is null";
        return false;
    }

    std::function<void()> operation = [this, account, subPath](){
        for (AccountWorkers* workers : this->generator()->accountWorkersVector()) {
            if (workers->account() != account)
                continue;

            CommandEntity* listingRequest = workers->browserCommandQueue()->directoryListingRequest(subPath, false, false);
            /*NcDirTreeCommandUnit* dirTreeRequest = new NcDirTreeCommandUnit(nullptr,
                                                                            workers->browserCommandQueue(),
                                                                            NODE_PATH_SEPARATOR,
                                                                            this->m_cachedTree,
                                                                            1);*/
            workers->browserCommandQueue()->enqueue(listingRequest);
            workers->browserCommandQueue()->run();
            break;
        }
    };

    if (this->thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(qApp, operation);
        return true;
    }

    operation();
    return true;
}
