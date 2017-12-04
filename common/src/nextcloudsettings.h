#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include "nextcloudsettingsbase.h"

#include <QSettings>

class NextcloudSettings : public NextcloudSettingsBase
{
    Q_OBJECT

public:
    static NextcloudSettings *instance();

    Q_PROPERTY(bool isCustomCert READ isCustomCert WRITE acceptCertificate NOTIFY customCertChanged)

public slots:
    void acceptCertificate(QString md5, QString sha1);
    void acceptCertificate(bool value);
    bool readSettings();
    void resetSettings();
    void writeSettings();

signals:
    void customCertChanged();

private:
    NextcloudSettings(QObject *parent = 0);
    bool isCustomCert();

    QSettings settings;
};
Q_DECLARE_METATYPE(NextcloudSettings*)

#endif // SETTINGS_H
