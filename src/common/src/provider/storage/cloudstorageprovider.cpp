#include "cloudstorageprovider.h"

CloudStorageProvider::CloudStorageProvider(QObject *parent, AccountBase* settings) :
    SettingsBackedCommandQueue(parent, settings)
{

}
