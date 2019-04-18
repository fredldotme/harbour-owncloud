#ifndef PERMITTEDSETTINGS_H
#define PERMITTEDSETTINGS_H

#include <QObject>

#include "nextcloudsettingsbase.h"

class PermittedSettings : public AccountBase
{
    Q_OBJECT

public:
    PermittedSettings(QObject *parent = 0);

public slots:
    bool readSettings();

};
Q_DECLARE_METATYPE(PermittedSettings*)

#endif // PERMITTEDSETTINGS_H
