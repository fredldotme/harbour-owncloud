#ifndef OCSCOMMANDQUEUE_H
#define OCSCOMMANDQUEUE_H

#include <QObject>
#include "commandqueue.h"
#include <settings/nextcloudsettingsbase.h>

class OcsCommandQueue : public CommandQueue
{
    Q_OBJECT

    Q_PROPERTY(NextcloudSettingsBase* settings READ settings WRITE setSettings NOTIFY settingsChanged)
public:
    explicit OcsCommandQueue(QObject *parent = nullptr,
                             NextcloudSettingsBase* settings = Q_NULLPTR);

    NextcloudSettingsBase* settings();
    void setSettings(NextcloudSettingsBase* v);

public slots:
    CommandEntity* userInfoRequest();

private:
    NextcloudSettingsBase* m_settings = Q_NULLPTR;

signals:
    void settingsChanged();

};

#endif // OCSCOMMANDQUEUE_H
