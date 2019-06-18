#include "sharingprovider.h"

SharingProvider::SharingProvider(QObject *parent, AccountBase* settings) :
    SettingsBackedCommandQueue(parent, settings)
{

}
