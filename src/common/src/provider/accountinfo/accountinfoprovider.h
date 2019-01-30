#ifndef ACCOUNTINFOPROVIDER_H
#define ACCOUNTINFOPROVIDER_H

#include <QObject>
#include <provider/settingsbackedcommandqueue.h>

class AccountInfoProvider : public SettingsBackedCommandQueue
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap userInfo READ userInfo NOTIFY userInfoChanged)
public:
    explicit AccountInfoProvider(QObject *parent = Q_NULLPTR,
                                 NextcloudSettingsBase* settings = Q_NULLPTR);

public slots:
    virtual CommandEntity* userInfoRequest() = 0;
    virtual QString providerSettingsUrl() = 0;

signals:
    void userInfoChanged();

private:
    QVariantMap userInfo() { return m_userInfo; }
    QVariantMap m_userInfo;
};

#endif // ACCOUNTINFOPROVIDER_H
