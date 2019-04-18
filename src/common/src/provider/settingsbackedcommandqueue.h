#ifndef SETTINGSBACKEDCOMMANDQUEUE_H
#define SETTINGSBACKEDCOMMANDQUEUE_H

#include <QObject>
#include <commandqueue.h>
#include <settings/nextcloudsettingsbase.h>

class SettingsBackedCommandQueue : public CommandQueue
{
    Q_OBJECT

    Q_PROPERTY(AccountBase* settings READ settings WRITE setSettings NOTIFY settingsChanged)

public:
    explicit SettingsBackedCommandQueue(QObject *parent = Q_NULLPTR,
                                        AccountBase* settings = Q_NULLPTR);

    AccountBase* settings();
    void setSettings(AccountBase* v);

private:
    AccountBase* m_settings = Q_NULLPTR;

signals:
    void settingsChanged();

};

#endif // SETTINGSBACKEDCOMMANDQUEUE_H
