CONFIG += qt c++11
QT += network xml sql

linux:!android {
    !contains(CONFIG, quickcontrols) {
        QT += dbus
    }
}
android {
    QT += androidextras
}

DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

CONFIG(release, debug|release) {
    QMAKE_POST_LINK=$(STRIP) $(TARGET)
    DEFINES += QT_NO_DEBUG_OUTPUT
}

contains(CONFIG, quickcontrols) {
    DEFINES += GHOSTCLOUD_UI_QUICKCONTROLS
}

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src
INCLUDEPATH += $$PWD/../../3rdparty/libqtcommandqueue/src
DEPENDPATH += $$PWD/../../3rdparty/libqtcommandqueue/src

INCLUDEPATH += $$PWD/../../3rdparty/qwebdavlib/qwebdavlib
DEPENDPATH += $$PWD/../../3rdparty/qwebdavlib/qwebdavlib

android {
    LIBS += $$OUT_PWD/../../3rdparty/openssl/libcrypto.so
    LIBS += $$OUT_PWD/../../3rdparty/openssl/libssl.so
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so
    LIBS += $$OUT_PWD/../../src/common/libharbourowncloudcommon.so
}

linux:!android {
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so.1
    LIBS += $$OUT_PWD/../../src/common/libharbourowncloudcommon.so.1
}

macx {
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.1.dylib
    PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.1.dylib

    libs.path = Contents/MacOS
    libs.files += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.1.dylib

    CONFIG(release, debug|release) {
        LIBS += $$OUT_PWD/../../src/common/libharbourowncloudcommon.1.dylib
        PRE_TARGETDEPS += $$OUT_PWD/../../src/common/libharbourowncloudcommon.1.dylib
        libs.files += $$OUT_PWD/../../src/common/libharbourowncloudcommon.1.dylib
    }
    CONFIG(debug, debug|release) {
        LIBS += $$OUT_PWD/../../src/common/libharbourowncloudcommon_debug.1.dylib
        PRE_TARGETDEPS += $$OUT_PWD/../../src/common/libharbourowncloudcommon_debug.1.dylib
        libs.files += $$OUT_PWD/../../src/common/libharbourowncloudcommon_debug.1.dylib
    }

    QMAKE_BUNDLE_DATA += libs
}

ios {
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.a
    CONFIG(release, debug|release) {
        LIBS += $$OUT_PWD/../../src/common/libharbourowncloudcommon.a
    }
    CONFIG(debug, debug|release) {
        LIBS += $$OUT_PWD/../../src/common/libharbourowncloudcommon_debug.a
    }
}

# Sailfish additionals
contains(CONFIG, sailfish_build) {
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so.1
    LIBS += $$OUT_PWD/../../src/common/libharbourowncloudcommon.so.1
}

# Ubuntu Touch configuration
contains(CONFIG, click) {
    DEFINES += GHOSTCLOUD_UBUNTU_TOUCH
    DEFINES += OS_SUPPORTS_THEME_PROVIDER # Qml-Ui-Set
}

QMAKE_RPATHDIR += /usr/share/harbour-owncloud/lib
