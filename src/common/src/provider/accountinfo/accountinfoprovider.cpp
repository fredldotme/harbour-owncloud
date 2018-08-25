#include "accountinfoprovider.h"

AccountInfoProvider::AccountInfoProvider(QObject *parent,
                                         NextcloudSettingsBase* settings) :
    SettingsBackedCommandQueue(parent, settings)
{

}
