#ifndef SETTINGSBACKEDCOMMANDQUEUE_H
#define SETTINGSBACKEDCOMMANDQUEUE_H

#include <QObject>
#include <commandqueue.h>
#include <settings/nextcloudsettingsbase.h>

class SettingsBackedCommandQueue : public CommandQueue
{
    Q_OBJECT

    Q_PROPERTY(NextcloudSettingsBase* settings READ settings WRITE setSettings NOTIFY settingsChanged)

public:
    explicit SettingsBackedCommandQueue(QObject *parent = Q_NULLPTR,
                                        NextcloudSettingsBase* settings = Q_NULLPTR);

    NextcloudSettingsBase* settings();
    void setSettings(NextcloudSettingsBase* v);

private:
    NextcloudSettingsBase* m_settings = Q_NULLPTR;

signals:
    void settingsChanged();

};

#endif // SETTINGSBACKEDCOMMANDQUEUE_H
