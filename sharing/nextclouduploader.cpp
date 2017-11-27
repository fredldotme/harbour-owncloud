#include "nextclouduploader.h"

ExampleUploader::ExampleUploader(QObject *parent):
    MediaTransferInterface(parent)
{

}

ExampleUploader::~ExampleUploader()
{
}

QString ExampleUploader::displayName() const
{
    return tr("Example");
}

QUrl ExampleUploader::serviceIcon() const
{
    // Url to the icon which should be shown in the transfer UI
    return QUrl("image://theme/icon-s-message");
}

bool ExampleUploader::cancelEnabled() const
{
    // Return true if cancelling ongoing upload is supported
    // Return false if cancelling ongoing upload is not supported
    return true;
}

bool ExampleUploader::restartEnabled() const
{
    // Return true, if restart is  supported.
    // Return false, if restart is not supported
    return true;
}

void ExampleUploader::start()
{
    // This is called by the sharing framework to start sharing

    // TODO: Add your code here to start uploading
}

void ExampleUploader::cancel()
{
    // This is called by the sharing framework to cancel on going transfer

    // TODO: Add your code here to cancel ongoing upload
}

