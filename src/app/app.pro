TARGET = harbour-owncloud

!contains(CONFIG, quickcontrols) {
    CONFIG += sailfishapp
    include($$PWD/../../3rdparty/sailfish-ui-set/sailfish-ui-set.pri)
    
    RESOURCES += \
        qml-sfos.qrc
}
contains(CONFIG, quickcontrols) {
    TEMPLATE = app
    
    QMAKE_RPATHDIR += $$OUT_PWD/../../3rdparty/qwebdav
    QMAKE_RPATHDIR += $$OUT_PWD/../common
    DEFINES += GHOSTCLOUD_UI_QUICKCONTROLS
    include($$PWD/../../3rdparty/qml-ui-set/qml-ui-set.pri)

    unix:!macx {
        desktop.files += harbour-owncloud.desktop
        desktop.path = /usr/share/applications
        target.path = /usr/bin
        INSTALLS += desktop target
    }

    ios {
        QMAKE_INFO_PLIST = $$PWD/ios/Info.plist
    }

    # OpenSSL for Android
    android {
        LIBS += -L $$OUT_PWD/../../3rdparty/openssl -lcrypto -lssl
        ANDROID_EXTRA_LIBS += $$OUT_PWD/../../3rdparty/openssl/libcrypto.so
        ANDROID_EXTRA_LIBS += $$OUT_PWD/../../3rdparty/openssl/libssl.so

        QT += androidextras
    }

    RESOURCES += \
        qml.qrc \
        theme.qrc
}

include($$PWD/../common/common.pri)
include($$PWD/../qmlcommon/qmlcommon.pri)

CONFIG += qt
QT += quick qml multimedia svg

SOURCES += \
    $$PWD/src/main.cpp \
    $$PWD/src/directorycontentmodel.cpp \
    $$PWD/src/ocsnetaccessfactory.cpp \
    $$PWD/src/webdavmediafeeder.cpp \
    $$PWD/src/accountworkers.cpp \
    $$PWD/src/accountworkergenerator.cpp

HEADERS += \
    $$PWD/src/directorycontentmodel.h \
    $$PWD/src/ocsnetaccessfactory.h \
    $$PWD/src/webdavmediafeeder.h \
    $$PWD/src/accountworkers.h \
    $$PWD/src/accountworkergenerator.h

# Daemon control classes
!contains(QT, dbus) {
    HEADERS += \
        $$PWD/src/daemonctrl/dummydaemonctrl.h

    SOURCES += \
        $$PWD/src/daemonctrl/dummydaemonctrl.cpp
}
contains(QT, dbus) {
    HEADERS += \
        $$PWD/src/daemonctrl/daemoncontrol.h

    SOURCES += \
        $$PWD/src/daemonctrl/daemoncontrol.cpp
}

# Intent-based file selection types
!contains(QT, androidextras) {
    SOURCES += \
        $$PWD/src/os/dummy/dummyintentfileselector.cpp

    HEADERS += \
        $$PWD/src/os/dummy/dummyintentfileselector.h
}
contains(QT, androidextras) {
    SOURCES += \
        $$PWD/src/os/android/intentfileselector.cpp \
        $$PWD/src/os/android/intentfileselectorreceiver.cpp

    HEADERS += \
        $$PWD/src/os/android/intentfileselector.h \
        $$PWD/src/os/android/intentfileselectorreceiver.h
}

# Consider whether to use native notification mechanisms on target OSes or not

RESOURCES += \
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

DISTFILES += \
    $$PWD/android/AndroidManifest.xml \
    $$PWD/android/gradle/wrapper/gradle-wrapper.jar \
    $$PWD/android/gradlew \
    $$PWD/android/res/values/libs.xml \
    $$PWD/android/build.gradle \
    $$PWD/android/gradle/wrapper/gradle-wrapper.properties \
    $$PWD/android/gradlew.bat \
    android/src/me/fredl/ghostcloud/ShareUtil.java

ANDROID_PACKAGE_SOURCE_DIR = \
    $$PWD/android
