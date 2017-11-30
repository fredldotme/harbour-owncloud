CONFIG += qt c++11
QT += dbus network xml
DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

SOURCES += \
    $$PWD/src/owncloudbrowser.cpp \
    $$PWD/src/transfermanager.cpp \
    $$PWD/src/entryinfo.cpp \
    $$PWD/src/settings.cpp \
    $$PWD/src/shellcommand.cpp \
    $$PWD/src/transferentry.cpp \
    $$PWD/src/webdav_utils.cpp

HEADERS += \
    $$PWD/src/owncloudbrowser.h \
    $$PWD/src/transfermanager.h \
    $$PWD/src/entryinfo.h \
    $$PWD/src/settings.h \
    $$PWD/src/shellcommand.h \
    $$PWD/src/transferentry.h \
    $$PWD/src/webdav_utils.h \
    $$PWD/src/ownclouddbusconsts.h

LIBS += $$OUT_PWD/../qwebdavlib/qwebdavlib/libqwebdav.so.1

QMAKE_RPATHDIR += /usr/share/harbour-owncloud/lib

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

INCLUDEPATH += $$PWD/../qwebdavlib/qwebdavlib
DEPENDPATH += $$PWD/../qwebdavlib/qwebdavlib

DISTFILES +=

RESOURCES += \
    $$PWD/qml.qrc
