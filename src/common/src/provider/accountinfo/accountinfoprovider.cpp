#include "accountinfoprovider.h"

AccountInfoProvider::AccountInfoProvider(QObject *parent,
                                         AccountBase* settings) :
    SettingsBackedCommandQueue(parent, settings)
{

}
