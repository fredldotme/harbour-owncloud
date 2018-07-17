#include "permissionrequestqueue.h"

#include <QDebug>
#include <QDBusConnection>
#include <nextcloudsettings.h>

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
    NextcloudSettings::instance()->readSettings();

    QMap<QString, QVariant> values;
    values.insert(NEXTCLOUD_SETTINGS_KEY_CERTMD5, NextcloudSettings::instance()->md5Hex());
    values.insert(NEXTCLOUD_SETTINGS_KEY_CERTSHA1, NextcloudSettings::instance()->sha1Hex());
    values.insert(NEXTCLOUD_SETTINGS_KEY_ISHTTPS, NextcloudSettings::instance()->isHttps());
    values.insert(NEXTCLOUD_SETTINGS_KEY_HOSTNAME, NextcloudSettings::instance()->hostname());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PORT, NextcloudSettings::instance()->port());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PATH, NextcloudSettings::instance()->path());
    values.insert(NEXTCLOUD_SETTINGS_KEY_USERNAME, NextcloudSettings::instance()->username());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PASSWORD, NextcloudSettings::instance()->password());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PICTURESPATH, NextcloudSettings::instance()->localPicturesPath());
    values.insert(NEXTCLOUD_SETTINGS_KEY_MOBILEUPLOAD, NextcloudSettings::instance()->mobileUpload());
    values.insert(NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS, NextcloudSettings::instance()->notifications());
    values.insert(NEXTCLOUD_SETTINGS_KEY_UPLOADAUTOMATICALLY, NextcloudSettings::instance()->uploadAutomatically());
    values.insert(NEXTCLOUD_PERMD_REQUESTDENIED, false);

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
