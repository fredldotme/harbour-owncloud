#include "globalowncloudbrowser.h"

GlobalOwncloudBrowser::GlobalOwncloudBrowser(QObject *parent) : OwnCloudBrowser(parent)
{

}

GlobalOwncloudBrowser* GlobalOwncloudBrowser::instance()
{
    static GlobalOwncloudBrowser browser;
    return &browser;
}
