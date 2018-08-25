#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

#include "nextcloudsettingsbase.h"

class IniFileSettings : public NextcloudSettingsBase
{
    Q_OBJECT

public:
    static IniFileSettings *instance();
    IniFileSettings(QObject *parent = 0);

public slots:
    bool readSettings();
    QString filePath();

private:
    QSettings m_settings;
};
Q_DECLARE_METATYPE(IniFileSettings*)

#endif // SETTINGS_H
