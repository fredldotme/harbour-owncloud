#include "permissionrequestqueue.h"

#include <QDebug>
#include <QDBusConnection>
#include <settings/inifilesettings.h>

PermissionRequestQueue::PermissionRequestQueue(QObject *parent) : QObject(parent)
{
}

void PermissionRequestQueue::enqueueRequest(QDBusMessage requestMessage)
{
    m_requestQueue.enqueue(requestMessage);
    emit permissionRequested();
}

void PermissionRequestQueue::permitFirstInQueue()
{
    if (m_requestQueue.length() < 1)
        return;

    QDBusMessage request = m_requestQueue.dequeue();

    // Refresh settings values
    //IniFileSettings::instance()->readSettings();

    QMap<QString, QVariant> values;
    /*values.insert(NEXTCLOUD_SETTINGS_KEY_CERTMD5, IniFileSettings::instance()->md5Hex());
    values.insert(NEXTCLOUD_SETTINGS_KEY_CERTSHA1, IniFileSettings::instance()->sha1Hex());
    values.insert(NEXTCLOUD_SETTINGS_KEY_ISHTTPS, IniFileSettings::instance()->isHttps());
    values.insert(NEXTCLOUD_SETTINGS_KEY_HOSTNAME, IniFileSettings::instance()->hostname());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PORT, IniFileSettings::instance()->port());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PATH, IniFileSettings::instance()->path());
    values.insert(NEXTCLOUD_SETTINGS_KEY_USERNAME, IniFileSettings::instance()->username());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PASSWORD, IniFileSettings::instance()->password());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PROVIDERTYPE, IniFileSettings::instance()->providerType());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PICTURESPATH, IniFileSettings::instance()->localPicturesPath());
    values.insert(NEXTCLOUD_SETTINGS_KEY_MOBILEUPLOAD, IniFileSettings::instance()->mobileUpload());
    values.insert(NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS, IniFileSettings::instance()->notifications());
    values.insert(NEXTCLOUD_SETTINGS_KEY_UPLOADAUTOMATICALLY, IniFileSettings::instance()->uploadAutomatically());
    values.insert(NEXTCLOUD_PERMD_REQUESTDENIED, false);*/

    QDBusMessage reply = request.createReply(values);
    QDBusConnection::sessionBus().send(reply);

    qInfo() << "Request permitted";

    // Check for the next request
    if (m_requestQueue.length() > 0)
        emit permissionRequested();
}

void PermissionRequestQueue::discardFirstInQueue()
{
    // Dequeue and return
    if (m_requestQueue.length() < 1)
        return;

    QDBusMessage request = m_requestQueue.dequeue();

    QMap<QString, QVariant> values;
    values.insert(NEXTCLOUD_PERMD_REQUESTDENIED, true);

    QDBusMessage reply = request.createReply(values);
    QDBusConnection::sessionBus().send(reply);
    qInfo() << "Request rejected";

    // Check for the next request
    if (m_requestQueue.length() > 0)
        emit permissionRequested();
}
