TARGET = harbour-owncloud-permission-agent

CONFIG = qt c++11 sailfishapp sailfish_build
QT = dbus quick xml gui-private

include($$PWD/../common/common.pri)

SOURCES += \
    main.cpp \
    dbushandler.cpp \
    dialogview.cpp \
    permissionrequestqueue.cpp

HEADERS += \
    dbushandler.h \
    dialogview.h \
    permissionrequestqueue.h

OTHER_FILES += harbour-owncloud-permission-agent.service

service.path = /usr/lib/systemd/user/
service.files += harbour-owncloud-permission-agent.service
binary.path = /usr/bin/
binary.files += $$OUT_PWD/harbour-owncloud-permission-agent

INSTALLS += service binary

RESOURCES += qml.qrc
