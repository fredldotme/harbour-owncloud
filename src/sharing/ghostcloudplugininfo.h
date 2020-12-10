#ifndef NEXTCLOUDPLUGININFO_H
#define NEXTCLOUDPLUGININFO_H

#include <TransferEngine-qt5/transferplugininfo.h>
#include <QList>

class GhostCloudPluginInfo : public TransferPluginInfo
{
    Q_OBJECT
public:
    GhostCloudPluginInfo();
    ~GhostCloudPluginInfo();

    QList<TransferMethodInfo> info() const;
    void query();
    bool ready() const;
private:
    QList<TransferMethodInfo> m_infoList;
    bool m_ready;
};

#endif // NEXTCLOUDPLUGININFO_H
