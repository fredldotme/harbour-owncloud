#ifndef EXAMINATIONSETTINGS_H
#define EXAMINATIONSETTINGS_H

#include <QObject>
#include "nextcloudsettingsbase.h"

class ExaminationSettings : public NextcloudSettingsBase
{
    Q_OBJECT
public:
    explicit ExaminationSettings(QObject *parent = nullptr);

public slots:
    bool setHostString(QString host);
};
Q_DECLARE_METATYPE(ExaminationSettings*)

#endif // EXAMINATIONSETTINGS_H
