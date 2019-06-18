#include "ocssharingcommandqueue.h"

OcsSharingCommandQueue::OcsSharingCommandQueue(QObject *parent,
                                               AccountBase* settings) :
    SharingProvider(parent, settings)
{

}

CommandEntity* OcsSharingCommandQueue::shareList()
{
    return Q_NULLPTR;
}
