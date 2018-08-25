CONFIG += qt c++11
QT += dbus network xml sql

DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

CONFIG(release, debug|release) {
    QMAKE_POST_LINK=$(STRIP) $(TARGET)
    DEFINES += QT_NO_DEBUG_OUTPUT
}

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

INCLUDEPATH += $$PWD/../../3rdparty/qwebdavlib/qwebdavlib
DEPENDPATH += $$PWD/../../3rdparty/qwebdavlib/qwebdavlib

LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so.1
LIBS += $$OUT_PWD/../../src/common/libharbourowncloudcommon.so.1

QMAKE_RPATHDIR += /usr/share/harbour-owncloud/lib
