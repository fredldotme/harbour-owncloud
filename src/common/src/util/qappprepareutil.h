#ifndef QAPPPREPAREUTIL_H
#define QAPPPREPAREUTIL_H

#include <QCoreApplication>

inline void prepareAppProperties(QCoreApplication& app)
{
#if !defined(GHOSTCLOUD_UI_QUICKCONTROLS)
    const QString QHOSTCLOUD_APP_NAME = QStringLiteral("harbour-owncloud");
#else
    const QString QHOSTCLOUD_APP_NAME = QStringLiteral("GhostCloud");
#endif

#if defined(GHOSTCLOUD_UBUNTU_TOUCH)
    app.setApplicationName(QStringLiteral("me.fredl.ghostcloud"));
#elif defined(GHOSTCLOUD_UBUNTU_TOUCH_PHOTOBACKUP)
    app.setApplicationName(QStringLiteral("me.fredl.ghostcloudphotobackup"));
#else
    app.setOrganizationName(QHOSTCLOUD_APP_NAME);
    app.setOrganizationDomain(QHOSTCLOUD_APP_NAME);
    app.setApplicationName(QHOSTCLOUD_APP_NAME);
#endif
}

#endif // QAPPPREPAREUTIL_H
