#ifndef QAPPPREPAREUTIL_H
#define QAPPPREPAREUTIL_H

#include <QCoreApplication>

inline void prepareAppProperties(QCoreApplication& app)
{
#ifndef GHOSTCLOUD_UI_QUICKCONTROLS
    const QString QHOSTCLOUD_APP_NAME = QStringLiteral("harbour-owncloud");
#else
    const QString QHOSTCLOUD_APP_NAME = QStringLiteral("GhostCloud");
#endif

#ifdef GHOSTCLOUD_UBUNTU_TOUCH
    app.setApplicationName(QStringLiteral("me.fredl.ghostcloud"));
#else
    app.setOrganizationName(QHOSTCLOUD_APP_NAME);
    app.setOrganizationDomain(QHOSTCLOUD_APP_NAME);
    app.setApplicationName(QHOSTCLOUD_APP_NAME);
#endif
}

#endif // QAPPPREPAREUTIL_H
