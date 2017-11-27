#include "nextclouduploader.h"

NextcloudUploader::NextcloudUploader(QObject *parent):
    MediaTransferInterface(parent)
{

}

NextcloudUploader::~NextcloudUploader()
{
}

QString NextcloudUploader::displayName() const
{
    return tr("Example");
}

QUrl NextcloudUploader::serviceIcon() const
{
    // Url to the icon which should be shown in the transfer UI
    return QUrl("image://theme/icon-s-message");
}

bool NextcloudUploader::cancelEnabled() const
{
    // Return true if cancelling ongoing upload is supported
    // Return false if cancelling ongoing upload is not supported
    return true;
}

bool NextcloudUploader::restartEnabled() const
{
    // Return true, if restart is  supported.
    // Return false, if restart is not supported
    return true;
}

void NextcloudUploader::start()
{
    // This is called by the sharing framework to start sharing

    // TODO: Add your code here to start uploading
}

void NextcloudUploader::cancel()
{
    // This is called by the sharing framework to cancel on going transfer

    // TODO: Add your code here to cancel ongoing upload
}

