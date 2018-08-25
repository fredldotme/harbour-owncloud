#ifndef OCSCOMMANDQUEUE_H
#define OCSCOMMANDQUEUE_H

#include <QObject>
#include <provider/accountinfo/accountinfoprovider.h>
#include <settings/nextcloudsettingsbase.h>

class OcsCommandQueue : public AccountInfoProvider
{
    Q_OBJECT

public:
    explicit OcsCommandQueue(QObject *parent = nullptr,
                             NextcloudSettingsBase* settings = Q_NULLPTR);

public slots:
    virtual CommandEntity* userInfoRequest() Q_DECL_OVERRIDE;

};

#endif // OCSCOMMANDQUEUE_H
