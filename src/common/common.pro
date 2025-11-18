TEMPLATE = lib
macx {
    CONFIG += lib_bundle
    TARGET = harbourowncloudcommon
}

!macx {
    TARGET = $$qtLibraryTarget(harbourowncloudcommon)
}

CONFIG += qt c++11
QT += network xml sql
QT -= gui

contains(CONFIG, qt6) {
    QT += core5compat
}

linux:!android {
    QT += dbus
}
android {
    QT += androidextras
}

DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

OTHER_FILES += $$PWD/common.pri

!macx {
    CONFIG(release, debug|release) {
        QMAKE_POST_LINK=$(STRIP) $(TARGET)
        DEFINES += QT_NO_DEBUG_OUTPUT
    }
}

SOURCES += \
    $$PWD/src/net/abstractfetcher.cpp \
    $$PWD/src/net/thumbnailfetcher.cpp \
    $$PWD/src/settings/nextcloudsettingsbase.cpp \
    $$PWD/src/settings/inifilesettings.cpp \
    $$PWD/src/util/filepathutil.cpp \
    $$PWD/src/util/shellcommand.cpp \
    $$PWD/src/util/webdav_utils.cpp \
    $$PWD/src/commands/webdav/fileuploadcommandentity.cpp \
    $$PWD/src/commands/webdav/filedownloadcommandentity.cpp \
    $$PWD/src/commands/webdav/webdavcommandentity.cpp \
    $$PWD/src/commands/webdav/mkdavdircommandentity.cpp \
    $$PWD/src/commands/webdav/davrmcommandentity.cpp \
    $$PWD/src/commands/webdav/davcopycommandentity.cpp \
    $$PWD/src/commands/webdav/davmovecommandentity.cpp \
    $$PWD/src/commands/webdav/davlistcommandentity.cpp \
    $$PWD/src/commands/http/httpcommandentity.cpp \
    $$PWD/src/commands/http/httpgetcommandentity.cpp \
    $$PWD/src/provider/storage/webdavcommandqueue.cpp \
    $$PWD/src/auth/qwebdavauthenticator.cpp \
    $$PWD/src/auth/authenticator.cpp \
    $$PWD/src/auth/authenticationexaminer.cpp \
    $$PWD/src/auth/flowloginauthenticator.cpp \
    $$PWD/src/commands/ocs/ocscommandentity.cpp \
    $$PWD/src/commands/ocs/ocsuserinfocommandentity.cpp \
    $$PWD/src/provider/accountinfo/ocscommandqueue.cpp \
    $$PWD/src/commands/webdav/davproppatchcommandentity.cpp \
    $$PWD/src/net/avatarfetcher.cpp \
    $$PWD/src/commands/nopcommandentity.cpp \
    $$PWD/src/commands/sync/ncdirtreecommandunit.cpp \
    $$PWD/src/commands/sync/ncsynccommandunit.cpp \
    $$PWD/src/cacheprovider.cpp \
    $$PWD/src/provider/storage/cloudstorageprovider.cpp \
    $$PWD/src/settings/db/syncdb.cpp \
    $$PWD/src/settings/db/accountdb.cpp \
    $$PWD/src/provider/settingsbackedcommandqueue.cpp \
    $$PWD/src/provider/accountinfo/accountinfoprovider.cpp \
    $$PWD/src/util/providerutils.cpp \
    $$PWD/src/accountworkers.cpp \
    $$PWD/src/accountworkergenerator.cpp \
    $$PWD/src/provider/sharing/sharingprovider.cpp \
    $$PWD/src/provider/sharing/ocssharingcommandqueue.cpp \
    $$PWD/src/commands/ocs/ocssharelistcommandentity.cpp \
    $$PWD/src/util/commandutil.cpp \
    src/sharedstatecontroller.cpp

HEADERS += \
    $$PWD/src/net/abstractfetcher.h \
    $$PWD/src/net/thumbnailfetcher.h \
    $$PWD/src/settings/nextcloudsettingsbase.h \
    $$PWD/src/settings/inifilesettings.h \
    $$PWD/src/util/filepathutil.h \
    $$PWD/src/util/shellcommand.h \
    $$PWD/src/util/webdav_utils.h \
    $$PWD/src/ownclouddbusconsts.h \
    $$PWD/src/commands/webdav/fileuploadcommandentity.h \
    $$PWD/src/commands/webdav/filedownloadcommandentity.h \
    $$PWD/src/commands/webdav/webdavcommandentity.h \
    $$PWD/src/commands/webdav/mkdavdircommandentity.h \
    $$PWD/src/commands/webdav/davrmcommandentity.h \
    $$PWD/src/commands/webdav/davcopycommandentity.h \
    $$PWD/src/commands/webdav/davmovecommandentity.h \
    $$PWD/src/commands/webdav/davlistcommandentity.h \
    $$PWD/src/commands/http/httpcommandentity.h \
    $$PWD/src/commands/http/httpgetcommandentity.h \
    $$PWD/src/provider/storage/webdavcommandqueue.h \
    $$PWD/src/nextcloudendpointconsts.h \
    $$PWD/src/auth/qwebdavauthenticator.h \
    $$PWD/src/auth/authenticator.h \
    $$PWD/src/auth/authenticationexaminer.h \
    $$PWD/src/auth/flowloginauthenticator.h \
    $$PWD/src/commands/ocs/ocscommandentity.h \
    $$PWD/src/commands/ocs/ocsuserinfocommandentity.h \
    $$PWD/src/provider/accountinfo/ocscommandqueue.h \
    $$PWD/src/commands/webdav/davproppatchcommandentity.h \
    $$PWD/src/net/avatarfetcher.h \
    $$PWD/src/commands/nopcommandentity.h \
    $$PWD/src/commands/sync/ncdirtreecommandunit.h \
    $$PWD/src/commands/sync/ncsynccommandunit.h \
    $$PWD/src/cacheprovider.h \
    $$PWD/src/provider/storage/cloudstorageprovider.h \
    $$PWD/src/settings/db/syncdb.h \
    $$PWD/src/settings/db/accountdb.h \
    $$PWD/src/provider/settingsbackedcommandqueue.h \
    $$PWD/src/provider/accountinfo/accountinfoprovider.h \
    $$PWD/src/util/providerutils.h \
    $$PWD/src/accountworkers.h \
    $$PWD/src/accountworkergenerator.h \
    $$PWD/src/provider/sharing/sharingprovider.h \
    $$PWD/src/util/qappprepareutil.h \
    $$PWD/src/provider/sharing/ocssharingcommandqueue.h \
    $$PWD/src/commands/ocs/ocssharelistcommandentity.h \
    $$PWD/src/util/commandutil.h \
    $$PWD/src/settings/db/accountsdbinterface.h \
    src/sharedstatecontroller.h

macx {
    include($$PWD/../../3rdparty/qwebdavlib/qwebdavlib/qwebdavlib.pri)
}
include($$PWD/../../3rdparty/libqtcommandqueue/libqtcommandqueue.pri)

contains(QT, dbus) {
    SOURCES += \
        $$PWD/src/settings/permittedsettings.cpp

    HEADERS += \
        $$PWD/src/settings/permittedsettings.h
}

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

INCLUDEPATH += $$PWD/../../3rdparty/qwebdavlib/qwebdavlib
DEPENDPATH += $$PWD/../../3rdparty/qwebdavlib/qwebdavlib

INCLUDEPATH += $$PWD/../../3rdparty/libqtcommandqueue/src
DEPENDPATH += $$PWD/../../3rdparty/libqtcommandqueue/src

# SailfishOS configuration
contains(CONFIG, sailfish_build) {
    QMAKE_RPATHDIR += /usr/share/harbour-owncloud/lib
    target.path = /usr/share/harbour-owncloud/lib
    qwebdavlib.path = /usr/share/harbour-owncloud/lib
}

# QtQuick.Controls-based implementations
contains(CONFIG, quickcontrols) {
    target.path = $$LIBDIR
    qwebdavlib.path = $$LIBDIR
}

# QWebDav
android {
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so
    qwebdavlib.files += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so
}
linux:!android {
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav-qt5.so.1
    qwebdavlib.files += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav-qt5.so.1
}
macx {
    CONFIG += app_extension_api_only
    CONFIG += force_debug_info

    FRAMEWORK_HEADERS_QWEBDAVLIB.version = Versions
    FRAMEWORK_HEADERS_QWEBDAVLIB.files = \
        $$files($$PWD/../../3rdparty/qwebdavlib/qwebdavlib/*.h)
    FRAMEWORK_HEADERS_QWEBDAVLIB.path = Headers
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_QWEBDAVLIB

    FRAMEWORK_HEADERS_COMMANDQUEU.version = Versions
    FRAMEWORK_HEADERS_COMMANDQUEU.files = \
        $$files($$PWD/../../3rdparty/libqtcommandqueue/src/*.h)
    FRAMEWORK_HEADERS_COMMANDQUEU.path = Headers
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_COMMANDQUEU

    FRAMEWORK_HEADERS_SRC.version = Versions
    FRAMEWORK_HEADERS_SRC.files = \
        $$files($$PWD/src/*.h)
    FRAMEWORK_HEADERS_SRC.path = Headers
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_SRC

    FRAMEWORK_HEADERS_AUTH.version = Versions
    FRAMEWORK_HEADERS_AUTH.files = \
        $$files($$PWD/src/auth/*.h)
    FRAMEWORK_HEADERS_AUTH.path = Headers/auth
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_AUTH

    FRAMEWORK_HEADERS_COMMANDS.version = Versions
    FRAMEWORK_HEADERS_COMMANDS.files = \
        $$files($$PWD/src/commands/*.h)
    FRAMEWORK_HEADERS_COMMANDS.path = Headers/commands
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_COMMANDS

    FRAMEWORK_HEADERS_COMMANDS_HTTP.version = Versions
    FRAMEWORK_HEADERS_COMMANDS_HTTP.files = \
        $$files($$PWD/src/commands/http/*.h)
    FRAMEWORK_HEADERS_COMMANDS_HTTP.path = Headers/commands/http
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_COMMANDS_HTTP

    FRAMEWORK_HEADERS_COMMANDS_OCS.version = Versions
    FRAMEWORK_HEADERS_COMMANDS_OCS.files = \
        $$files($$PWD/src/commands/ocs/*.h)
    FRAMEWORK_HEADERS_COMMANDS_OCS.path = Headers/commands/ocs
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_COMMANDS_OCS

    FRAMEWORK_HEADERS_COMMANDS_SYNC.version = Versions
    FRAMEWORK_HEADERS_COMMANDS_SYNC.files = \
        $$files($$PWD/src/commands/sync/*.h)
    FRAMEWORK_HEADERS_COMMANDS_SYNC.path = Headers/commands/sync
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_COMMANDS_SYNC

    FRAMEWORK_HEADERS_COMMANDS_WEBDAV.version = Versions
    FRAMEWORK_HEADERS_COMMANDS_WEBDAV.files = \
        $$files($$PWD/src/commands/webdav/*.h)
    FRAMEWORK_HEADERS_COMMANDS_WEBDAV.path = Headers/commands/webdav
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_COMMANDS_WEBDAV

    FRAMEWORK_HEADERS_NET.version = Versions
    FRAMEWORK_HEADERS_NET.files = \
        $$files($$PWD/src/net/*.h)
    FRAMEWORK_HEADERS_NET.path = Headers/net
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_NET

    FRAMEWORK_HEADERS_PROV.version = Versions
    FRAMEWORK_HEADERS_PROV.files = \
        $$files($$PWD/src/provider/*.h)
    FRAMEWORK_HEADERS_PROV.path = Headers/provider
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_PROV

    FRAMEWORK_HEADERS_PROV_ACC.version = Versions
    FRAMEWORK_HEADERS_PROV_ACC.files = \
        $$files($$PWD/src/provider/accountinfo/*.h)
    FRAMEWORK_HEADERS_PROV_ACC.path = Headers/provider/accountinfo
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_PROV_ACC

    FRAMEWORK_HEADERS_PROV_SHARE.version = Versions
    FRAMEWORK_HEADERS_PROV_SHARE.files = \
        $$files($$PWD/src/provider/sharing/*.h)
    FRAMEWORK_HEADERS_PROV_SHARE.path = Headers/provider/sharing
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_PROV_SHARE

    FRAMEWORK_HEADERS_PROV_STORE.version = Versions
    FRAMEWORK_HEADERS_PROV_STORE.files = \
        $$files($$PWD/src/provider/storage/*.h)
    FRAMEWORK_HEADERS_PROV_STORE.path = Headers/provider/storage
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_PROV_STORE

    FRAMEWORK_HEADERS_SETTINGS.version = Versions
    FRAMEWORK_HEADERS_SETTINGS.files = \
        $$files($$PWD/src/settings/*.h)
    FRAMEWORK_HEADERS_SETTINGS.path = Headers/settings
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_SETTINGS

    FRAMEWORK_HEADERS_SETTINGS_DB.version = Versions
    FRAMEWORK_HEADERS_SETTINGS_DB.files = \
        $$files($$PWD/src/settings/db/*.h)
    FRAMEWORK_HEADERS_SETTINGS_DB.path = Headers/settings/db
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_SETTINGS_DB

    FRAMEWORK_HEADERS_UTIL.version = Versions
    FRAMEWORK_HEADERS_UTIL.files = \
        $$files($$PWD/src/util/*.h)
    FRAMEWORK_HEADERS_UTIL.path = Headers/util
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS_UTIL

    FRAMEWORK_PLUGINS_SQLITE.version = Versions
    FRAMEWORK_PLUGINS_SQLITE.files = $(QTDIR)/plugins/sqldrivers/libqsqlite.dylib
    FRAMEWORK_PLUGINS_SQLITE.path = PlugIns/sqldrivers
    QMAKE_BUNDLE_DATA += FRAMEWORK_PLUGINS_SQLITE

    QMAKE_SONAME_PREFIX = @rpath
    QMAKE_POST_LINK += $$quote(codesign -s \"Apple Developer\" $$OUT_PWD/$${TARGET}.framework/Versions/Current/PlugIns/sqldrivers/libqsqlite.dylib);
}

# Ubuntu Touch configuration
contains(CONFIG, click) {
    DEFINES += GHOSTCLOUD_UBUNTU_TOUCH
    CONFIG += ubuntutouch
}

contains(CONFIG, clickphotobackup) {
    DEFINES += GHOSTCLOUD_UBUNTU_TOUCH_PHOTOBACKUP
    CONFIG += ubuntutouch
}

contains(CONFIG, ubuntutouch) {
    SOURCES += \
        $$PWD/src/commands/ubuntutouch/utfiledownloadcommandentity.cpp \
        #$$PWD/src/commands/ubuntutouch/utfileuploadcommandentity.cpp \
        $$PWD/src/settings/db/utaccountsdb.cpp
    HEADERS += \
        $$PWD/src/commands/ubuntutouch/utfiledownloadcommandentity.h \
        #$$PWD/src/commands/ubuntutouch/utfileuploadcommandentity.h \
        $$PWD/src/settings/db/utaccountsdb.h
    INCLUDEPATH += \
        /usr/include/lomiri-online-accounts-qt5
    DEPENDPATH += \
        /usr/include/lomiri-online-accounts-qt5
    LIBS += -llomiri-download-manager-client -llomiri-online-accounts-qt5
}

INSTALLS += qwebdavlib target
