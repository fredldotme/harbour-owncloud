#ifndef NEXTCLOUDENDPOINTCONSTS_H
#define NEXTCLOUDENDPOINTCONSTS_H

#include <QObject>
#include <QString>

const QString NEXTCLOUD_ENDPOINT_WEBDAV = QStringLiteral("remote.php/webdav");
const QString NEXTCLOUD_ENDPOINT_LOGIN_FLOW = QStringLiteral("index.php/login/flow");
const QString NEXTCLOUD_ENDPOINT_THUMBNAIL = QStringLiteral("index.php/apps/files/api/v1/thumbnail");
const QString NEXTCLOUD_ENDPOINT_AVATAR = QStringLiteral("index.php/avatar/%1/%2");
const QString NEXTCLOUD_ENDPOINT_OCS_USERINFO = QStringLiteral("ocs/v2.php/cloud/users/");
const QString NEXTCLOUD_ENDPOINT_OCS_SETTINGS = QStringLiteral("index.php/settings/user");
const QString NEXTCLOUD_ENDPOINT_OCS_SHARE = QStringLiteral("ocs/v2.php/apps/files_sharing/api/v1");
const QString NEXTCLOUD_ENDPOINT_OCS_SHARE_LIST = NEXTCLOUD_ENDPOINT_OCS_SHARE + QStringLiteral("/shares");
const QString NEXTCLOUD_ENDPOINT_OCS_SHARE_INFO = NEXTCLOUD_ENDPOINT_OCS_SHARE + QStringLiteral("/shares/%1");
const QString NEXTCLOUD_ENDPOINT_OCS_SHARE_NEW = NEXTCLOUD_ENDPOINT_OCS_SHARE + QStringLiteral("/shares");
const QString NEXTCLOUD_ENDPOINT_OCS_SHARE_DELETE = NEXTCLOUD_ENDPOINT_OCS_SHARE + QStringLiteral("/shares/%1");

#endif // NEXTCLOUDENDPOINTCONSTS_H
