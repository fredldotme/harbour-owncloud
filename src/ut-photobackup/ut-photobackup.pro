TARGET = harbour-owncloud-photobackup
TEMPLATE = app

QMAKE_RPATHDIR += $$OUT_PWD/../../3rdparty/qwebdav
QMAKE_RPATHDIR += $$OUT_PWD/../common

target.path = /usr/bin
libwebdav.files += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so.1
libwebdav.path = /usr/lib
libcommon.files += $$OUT_PWD/../common/libharbourowncloudcommon.so.1
libcommon.path = /usr/lib

INSTALLS += target libwebdav libcommon

include($$PWD/../common/common.pri)

CONFIG += qt
QT += quick qml dbus

SOURCES += \
    $$PWD/src/main.cpp \
    $$PWD/src/servicecreator.cpp \
    $$PWD/src/daemoncontrol.cpp

HEADERS += \
    $$PWD/src/servicecreator.h \
    $$PWD/src/daemoncontrol.h

RESOURCES += \
    $$PWD/qml.qrc \
    $$PWD/icons.qrc
