CONFIG += qt c++11
QT += network xml sql

linux:!android {
    QT += dbus
}
android {
    QT += androidextras
}

DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

CONFIG(release, debug|release) {
    QMAKE_POST_LINK=$(STRIP) $(TARGET)
    DEFINES += QT_NO_DEBUG_OUTPUT
}

contains(CONFIG, quickcontrols): DEFINES += GHOSTCLOUD_UI_QUICKCONTROLS

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

INCLUDEPATH += $$PWD/../../3rdparty/qwebdavlib/qwebdavlib
DEPENDPATH += $$PWD/../../3rdparty/qwebdavlib/qwebdavlib

android {
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so
    LIBS += $$OUT_PWD/../../src/common/libharbourowncloudcommon.so
}

linux:!android {
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so.1
    LIBS += $$OUT_PWD/../../src/common/libharbourowncloudcommon.so.1
}

QMAKE_RPATHDIR += /usr/share/harbour-owncloud/lib
