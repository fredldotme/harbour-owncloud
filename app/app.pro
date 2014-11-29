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
QT += xml

SOURCES += src/harbour-owncloud.cpp \
    src/owncloudbrowser.cpp \
    src/entryinfo.cpp \
    src/settings.cpp \
    src/downloadmanager.cpp \
    src/downloadentry.cpp

HEADERS += \
    src/owncloudbrowser.h \
    src/entryinfo.h \
    src/settings.h \
    src/downloadmanager.h \
    src/downloadentry.h

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
    qml/pages/SSLErrorDialog.qml

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/harbour-owncloud-de.ts

LIBS += $$OUT_PWD/../qwebdavlib/qwebdavlib/libqwebdav.so.1

QMAKE_RPATHDIR += /usr/share/harbour-owncloud/libs

qwebdavlib.path = /usr/share/harbour-owncloud/libs
qwebdavlib.files += $$OUT_PWD/../qwebdavlib/qwebdavlib/libqwebdav.so.1
INSTALLS += qwebdavlib

INCLUDEPATH += $$PWD/../qwebdavlib/qwebdavlib
DEPENDPATH += $$PWD/../qwebdavlib/qwebdavlib

DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES
