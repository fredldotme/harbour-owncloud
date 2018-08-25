#include "cloudstorageprovider.h"

CloudStorageProvider::CloudStorageProvider(QObject *parent, NextcloudSettingsBase* settings) :
    SettingsBackedCommandQueue(parent, settings)
{

}
