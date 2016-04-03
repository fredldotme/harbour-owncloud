# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-owncloud

CONFIG += sailfishapp
QT += xml dbus

SOURCES += src/harbour-owncloud.cpp \
    src/owncloudbrowser.cpp \
    src/entryinfo.cpp \
    src/shellcommand.cpp \
    src/transferentry.cpp \
    src/transfermanager.cpp \
    src/localfilebrowser.cpp \
    ../common/src/settings.cpp \
    src/daemoncontrol.cpp

HEADERS += \
    src/owncloudbrowser.h \
    src/entryinfo.h \
    src/shellcommand.h \
    src/transferentry.h \
    src/transfermanager.h \
    src/localfilebrowser.h \
    ../common/src/settings.h \
    src/daemoncontrol.h

OTHER_FILES += qml/harbour-owncloud.qml \
    qml/cover/CoverPage.qml \
    rpm/harbour-owncloud.changes.in \
    rpm/harbour-owncloud.spec \
    rpm/harbour-owncloud.yaml \
    translations/*.ts \
    harbour-owncloud.desktop \
    qml/pages/FileBrowser.qml \
    qml/pages/FileDetails.qml \
    qml/pages/Login.qml \
    qml/pages/SSLErrorDialog.qml \
    qml/pages/SettingsPage.qml \
    qml/pages/TransferPage.qml \
    qml/pages/UploadDialog.qml \
    qml/pages/MkDirDialog.qml

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/harbour-owncloud-de.ts


icon86.files += icons/86x86/harbour-owncloud.png
icon86.path = /usr/share/icons/hicolor/86x86/apps
INSTALLS += icon86

#icon108.files += icons/108x108/harbour-owncloud.png
#icon108.path = /usr/share/icons/hicolor/108x108/apps
#INSTALLS += icon108

icon128.files += icons/128x128/harbour-owncloud.png
icon128.path = /usr/share/icons/hicolor/128x128/apps
INSTALLS += icon128

#icon256.files += icons/256x256/harbour-owncloud.png
#icon256.path = /usr/share/icons/hicolor/256x256/apps
#INSTALLS += icon256

#iconsvg.files += icons/scalable/harbour-owncloud.svgz
#iconsvg.path = /usr/share/icons/hicolor/scalable/apps

include(../notifications.pri)

LIBS += $$OUT_PWD/../qwebdavlib/qwebdavlib/libqwebdav.so.1

QMAKE_RPATHDIR += /usr/share/harbour-owncloud/lib

qwebdavlib.path = /usr/share/harbour-owncloud/lib
qwebdavlib.files += $$OUT_PWD/../qwebdavlib/qwebdavlib/libqwebdav.so.1
INSTALLS += qwebdavlib

INCLUDEPATH += $$PWD/../qwebdavlib/qwebdavlib
DEPENDPATH += $$PWD/../qwebdavlib/qwebdavlib

INCLUDEPATH += $$PWD/../common/src
DEPENDPATH += $$PWD/../common/src

DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES
