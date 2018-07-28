CONFIG += qt c++11
QT += dbus network xml

DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

INCLUDEPATH += $$PWD/../../3rdparty/qwebdavlib/qwebdavlib
DEPENDPATH += $$PWD/../../3rdparty/qwebdavlib/qwebdavlib

LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so.1
LIBS += $$OUT_PWD/../../src/common/libharbourowncloudcommon.so.1

QMAKE_RPATHDIR += /usr/share/harbour-owncloud/lib
