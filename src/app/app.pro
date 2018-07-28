TARGET = harbour-owncloud

CONFIG += sailfishapp qt
QT += xml dbus quick qml

include($$PWD/../common/common.pri)
include($$PWD/../qmlcommon/qmlcommon.pri)
include($$PWD/../../3rdparty/sailfish-ui-set/sailfish-ui-set.pri)

SOURCES += \
    src/harbour-owncloud.cpp \
    src/daemoncontrol.cpp \
    src/directorycontentmodel.cpp \
    src/ocsnetaccessfactory.cpp

HEADERS += \
    src/daemoncontrol.h \
    src/directorycontentmodel.h \
    src/ocsnetaccessfactory.h

RESOURCES += \
    qml.qrc \
    icons.qrc

OTHER_FILES += \
    translations/*.ts \
    harbour-owncloud.desktop

lupdate_only {
    OTHER_FILES += \
        qml/harbour-owncloud.qml \
        qml/cover/CoverPage.qml \
        qml/pages/FileBrowser.qml \
        qml/pages/FileDetails.qml \
        qml/pages/Login.qml \
        qml/pages/SSLErrorDialog.qml \
        qml/pages/SettingsPage.qml \
        qml/pages/TransferPage.qml \
        qml/pages/RemoteDirSelectDialog.qml
}

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += \
    translations/harbour-owncloud-de.ts \
    translations/harbour-owncloud-sl.ts \
    translations/harbour-owncloud-fr.ts

icon86.files += icons/86x86/harbour-owncloud.png
icon86.path = /usr/share/icons/hicolor/86x86/apps

icon128.files += icons/128x128/harbour-owncloud.png
icon128.path = /usr/share/icons/hicolor/128x128/apps

INSTALLS += icon86 icon128

DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

