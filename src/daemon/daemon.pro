TARGET = harbour-owncloud-daemon

CONFIG = qt c++11
QT = dbus network xml
DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

include(../common/common.pri)

SOURCES += \
    main.cpp \
    filesystem.cpp \
    networkmonitor.cpp \
    dbushandler.cpp

HEADERS += \
    filesystem.h \
    networkmonitor.h \
    dbushandler.h

OTHER_FILES += harbour-owncloud-daemon.service

service.path = /usr/lib/systemd/user/
service.files += harbour-owncloud-daemon.service
target.path = /usr/bin/

INSTALLS += target service
