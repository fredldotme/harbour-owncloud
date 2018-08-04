#ifndef NEXTCLOUDENDPOINTCONSTS_H
#define NEXTCLOUDENDPOINTCONSTS_H

#include <QObject>
#include <QString>

const QString NEXTCLOUD_ENDPOINT_WEBDAV = QStringLiteral("remote.php/webdav");
const QString NEXTCLOUD_ENDPOINT_LOGIN_FLOW = QStringLiteral("index.php/login/flow");
const QString NEXTCLOUD_ENDPOINT_THUMBNAIL = QStringLiteral("index.php/apps/files/api/v1/thumbnail");
const QString NEXTCLOUD_ENDPOINT_AVATAR = QStringLiteral("index.php/avatar/%1/%2");
const QString NEXTCLOUD_ENDPOINT_OCS_USERINFO = QStringLiteral("ocs/v2.php/cloud/users/");

/*class NextcloudApiEndpoints {
    Q_GADGET

    Q_PROPERTY(QString webdav MEMBER webdav CONSTANT)
    Q_PROPERTY(QString loginFlow MEMBER  CONSTANT)
    Q_PROPERTY(QString thumbnail MEMBER NEXTCLOUD_ENDPOINT_THUMBNAIL CONSTANT)
};
Q_DECLARE_METATYPE(NextcloudApiEndpoints)*/

#endif // NEXTCLOUDENDPOINTCONSTS_H
