#ifndef SHARINGPROVIDER_H
#define SHARINGPROVIDER_H

#include <provider/settingsbackedcommandqueue.h>
#include <QVariantList>

class SharingProvider : public SettingsBackedCommandQueue
{
    Q_OBJECT

    Q_PROPERTY(QVariantList shares READ shares NOTIFY sharesChanged)

public:
    explicit SharingProvider(QObject *parent = Q_NULLPTR,
                             AccountBase* settings = Q_NULLPTR);

    virtual CommandEntity* shareList() = 0;
    QVariantList shares() { return this->m_shares; }

private:
    QVariantList m_shares;

signals:
    void sharesChanged();
};

#endif // SHARINGPROVIDER_H
