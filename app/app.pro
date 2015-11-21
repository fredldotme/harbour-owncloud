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
    src/transfermanager.cpp \
    src/localfilebrowser.cpp \
    ../common/src/settings.cpp \
    ../common/src/shellcommand.cpp \
    ../common/src/transferentry.cpp \
    ../common/src/webdav_utils.cpp \
    src/daemoncontrol.cpp

HEADERS += \
    src/owncloudbrowser.h \
    src/entryinfo.h \
    src/transfermanager.h \
    src/localfilebrowser.h \
    ../common/src/settings.h \
    ../common/src/shellcommand.h \
    ../common/src/transferentry.h \
    ../common/src/webdav_utils.h \
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
