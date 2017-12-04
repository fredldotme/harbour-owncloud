TARGET = harbour-owncloud-permissiond

CONFIG = qt c++11 sailfishapp

include(../common/common.pri)
QT = dbus quick xml gui-private

SOURCES += \
    main.cpp \
    dbushandler.cpp \
    dialogview.cpp \
    permissionrequestqueue.cpp

HEADERS += \
    dbushandler.h \
    dialogview.h \
    permissionrequestqueue.h

OTHER_FILES += harbour-owncloud-permissiond.service

service.path = /usr/lib/systemd/user/
service.files += harbour-owncloud-permissiond.service
binary.path = /usr/bin/
binary.files += $$OUT_PWD/harbour-owncloud-permissiond

INSTALLS += service binary

RESOURCES += qml.qrc
