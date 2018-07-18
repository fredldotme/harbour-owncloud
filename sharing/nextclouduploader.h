#ifndef NEXTCLOUDUPLOADER_H
#define NEXTCLOUDUPLOADER_H

#include <TransferEngine-qt5/mediatransferinterface.h>
#include <TransferEngine-qt5/mediaitem.h>
#include <settings/nextcloudsettingsbase.h>

class NextcloudUploader : public MediaTransferInterface
{
    Q_OBJECT
public:
    NextcloudUploader(QObject *parent = 0);
    ~NextcloudUploader();

    QString displayName() const;
    QUrl serviceIcon() const;
    bool cancelEnabled() const;
    bool restartEnabled() const;

public Q_SLOTS:
    void start();
    void cancel();
};

#endif // NEXTCLOUDUPLOADER_H
