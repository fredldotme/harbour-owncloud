#include "permissionrequestqueue.h"

#include <QDebug>
#include <QDBusConnection>

PermissionRequestQueue::PermissionRequestQueue(QObject *parent) : QObject(parent)
{
}

void PermissionRequestQueue::enqueueRequest(QDBusMessage requestMessage)
{
    m_requestQueue.enqueue(requestMessage);
    emit permissionRequested();
}

void PermissionRequestQueue::permitFirstInQueue(AccountBase* account)
{
    if (m_requestQueue.length() < 1)
        return;

    QDBusMessage request = m_requestQueue.dequeue();

    QMap<QString, QVariant> values;
    values.insert(NEXTCLOUD_SETTINGS_KEY_CERTMD5, account->md5Hex());
    values.insert(NEXTCLOUD_SETTINGS_KEY_CERTSHA1, account->sha1Hex());
    values.insert(NEXTCLOUD_SETTINGS_KEY_ISHTTPS, account->isHttps());
    values.insert(NEXTCLOUD_SETTINGS_KEY_HOSTNAME, account->hostname());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PORT, account->port());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PATH, account->path());
    values.insert(NEXTCLOUD_SETTINGS_KEY_USERNAME, account->username());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PASSWORD, account->password());
    values.insert(NEXTCLOUD_SETTINGS_KEY_PROVIDERTYPE, account->providerType());
    values.insert(NEXTCLOUD_SETTINGS_KEY_MOBILEUPLOAD, account->mobileUpload());
    values.insert(NEXTCLOUD_SETTINGS_KEY_NOTIFICATIONS, account->notifications());
    values.insert(NEXTCLOUD_SETTINGS_KEY_UPLOADAUTOMATICALLY, account->uploadAutomatically());
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
