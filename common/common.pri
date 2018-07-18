CONFIG += qt c++11
QT += dbus network xml
DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

SOURCES += \
    $$PWD/src/settings/nextcloudsettingsbase.cpp \
    $$PWD/src/settings/nextcloudsettings.cpp \
    $$PWD/src/settings/permittedsettings.cpp \
    $$PWD/src/net/owncloudbrowser.cpp \
    $$PWD/src/net/transfermanager.cpp \
    $$PWD/src/net/transferentry.cpp \
    $$PWD/src/net/remotefileinfo.cpp \
    $$PWD/src/net/webdav_utils.cpp \
    $$PWD/src/util/shellcommand.cpp \
    $$PWD/src/commandqueue.cpp \
    $$PWD/src/commandentity.cpp \
    $$PWD/src/commands/fileuploadcommandentity.cpp \
    $$PWD/src/commands/filedownloadcommandentity.cpp \
    $$PWD/src/commands/remotedirectorycommandentity.cpp \
    $$PWD/src/net/thumbnailfetcher.cpp

HEADERS += \
    $$PWD/src/settings/nextcloudsettingsbase.h \
    $$PWD/src/settings/nextcloudsettings.h \
    $$PWD/src/settings/permittedsettings.h \
    $$PWD/src/net/owncloudbrowser.h \
    $$PWD/src/net/transfermanager.h \
    $$PWD/src/net/transferentry.h \
    $$PWD/src/net/remotefileinfo.h \
    $$PWD/src/net/webdav_utils.h \
    $$PWD/src/util/shellcommand.h \
    $$PWD/src/ownclouddbusconsts.h \
    $$PWD/src/commandqueue.h \
    $$PWD/src/commandentityinfo.h \
    $$PWD/src/commandentity.h \
    $$PWD/src/commands/fileuploadcommandentity.h \
    $$PWD/src/commands/filedownloadcommandentity.h \
    $$PWD/src/commands/remotedirectorycommandentity.h \
    $$PWD/src/net/thumbnailfetcher.h

LIBS += $$OUT_PWD/../qwebdavlib/qwebdavlib/libqwebdav.so.1

QMAKE_RPATHDIR += /usr/share/harbour-owncloud/lib

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

INCLUDEPATH += $$PWD/../qwebdavlib/qwebdavlib
DEPENDPATH += $$PWD/../qwebdavlib/qwebdavlib
