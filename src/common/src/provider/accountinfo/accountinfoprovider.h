#ifndef ACCOUNTINFOPROVIDER_H
#define ACCOUNTINFOPROVIDER_H

#include <QObject>
#include <provider/settingsbackedcommandqueue.h>

class AccountInfoProvider : public SettingsBackedCommandQueue
{
    Q_OBJECT
public:
    explicit AccountInfoProvider(QObject *parent = Q_NULLPTR,
                                 NextcloudSettingsBase* settings = Q_NULLPTR);

public slots:
    virtual CommandEntity* userInfoRequest() = 0;

};

#endif // ACCOUNTINFOPROVIDER_H
