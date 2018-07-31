#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

#include "nextcloudsettingsbase.h"

class NextcloudSettings : public NextcloudSettingsBase
{
    Q_OBJECT

public:
    static NextcloudSettings *instance();
    NextcloudSettings(QObject *parent = 0);

public slots:
    bool readSettings() Q_DECL_OVERRIDE;
    void writeSettings() Q_DECL_OVERRIDE;
    void resetSettings();

private:
    QSettings settings;
};
Q_DECLARE_METATYPE(NextcloudSettings*)

#endif // SETTINGS_H
