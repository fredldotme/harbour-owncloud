#ifndef GLOBALOWNCLOUDBROWSER_H
#define GLOBALOWNCLOUDBROWSER_H

#include <owncloudbrowser.h>

class GlobalOwncloudBrowser : public OwnCloudBrowser
{
    Q_OBJECT
public:
    explicit GlobalOwncloudBrowser(QObject *parent = 0);
    static GlobalOwncloudBrowser* instance();
};

#endif // GLOBALOWNCLOUDBROWSER_H
