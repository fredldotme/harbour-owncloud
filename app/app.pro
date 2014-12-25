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
    src/settings.cpp \
    src/shellcommand.cpp \
    src/transferentry.cpp \
    src/transfermanager.cpp \
    src/localfilebrowser.cpp

HEADERS += \
    src/owncloudbrowser.h \
    src/entryinfo.h \
    src/settings.h \
    src/shellcommand.h \
    src/transferentry.h \
    src/transfermanager.h \
    src/localfilebrowser.h

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

LIBS += $$OUT_PWD/../qwebdavlib/qwebdavlib/libqwebdav.so.1
LIBS += $$OUT_PWD/../nemo-qml-plugin-notifications/src/libnemonotifications-qt5.so.0

QMAKE_RPATHDIR += /usr/share/harbour-owncloud/lib

qwebdavlib.path = /usr/share/harbour-owncloud/lib
qwebdavlib.files += $$OUT_PWD/../qwebdavlib/qwebdavlib/libqwebdav.so.1

notifications.path = /usr/share/harbour-owncloud/lib
notifications.files += $$OUT_PWD/../nemo-qml-plugin-notifications/src/libnemonotifications-qt5.so.0
INSTALLS += qwebdavlib notifications

INCLUDEPATH += $$PWD/../qwebdavlib/qwebdavlib
DEPENDPATH += $$PWD/../qwebdavlib/qwebdavlib

INCLUDEPATH += $$PWD/../nemo-qml-plugin-notifications/src
DEPENDPATH += $$PWD/../nemo-qml-plugin-notifications/src

DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES
