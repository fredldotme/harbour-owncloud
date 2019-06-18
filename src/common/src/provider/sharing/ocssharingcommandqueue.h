#ifndef OCSSHARINGCOMMANDQUEUE_H
#define OCSSHARINGCOMMANDQUEUE_H

#include <provider/sharing/sharingprovider.h>

#include <QVariantList>

class OcsSharingCommandQueue : public SharingProvider
{
    Q_OBJECT

public:
    explicit OcsSharingCommandQueue(QObject *parent = Q_NULLPTR,
                                    AccountBase* settings = Q_NULLPTR);

    virtual CommandEntity* shareList() Q_DECL_OVERRIDE;

signals:

public slots:
};

#endif // OCSSHARINGCOMMANDQUEUE_H
