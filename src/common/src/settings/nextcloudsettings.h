#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

#include "nextcloudsettingsbase.h"

class NextcloudSettings : public NextcloudSettingsBase
{
    Q_OBJECT

    Q_PROPERTY(bool isCustomCert READ isCustomCert WRITE acceptCertificate NOTIFY customCertChanged)
public:
    static NextcloudSettings *instance();
    NextcloudSettings(QObject *parent = 0);

public slots:
    void acceptCertificate(QString md5, QString sha1);
    void acceptCertificate(bool value);
    bool readSettings();
    void resetSettings();
    void writeSettings();

signals:
    void customCertChanged();

private:
    bool isCustomCert();

    QSettings settings;
};
Q_DECLARE_METATYPE(NextcloudSettings*)

#endif // SETTINGS_H
