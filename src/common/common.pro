TEMPLATE = lib
TARGET = $$qtLibraryTarget(harbourowncloudcommon)

CONFIG += qt c++11
QT += network xml sql

linux:!android {
    QT += dbus
}
android {
    QT += androidextras
}

DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

OTHER_FILES += $$PWD/common.pri

CONFIG(release, debug|release) {
    QMAKE_POST_LINK=$(STRIP) $(TARGET)
    DEFINES += QT_NO_DEBUG_OUTPUT
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
    $$PWD/src/util/commandutil.cpp

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
    src/settings/db/accountsdbinterface.h

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

INCLUDEPATH += $$PWD/../../3rdparty/libqtcommandqueue/src
DEPENDPATH += $$PWD/../../3rdparty/libqtcommandqueue/src

# SailfishOS configuration
!contains(CONFIG, quickcontrols) {
    QMAKE_RPATHDIR += /usr/share/harbour-owncloud/lib
    target.path = /usr/share/harbour-owncloud/lib
    qwebdavlib.path = /usr/share/harbour-owncloud/lib
}

# QtQuick.Controls-based implementations
contains(CONFIG, quickcontrols) {
    target.path = /usr/lib
    qwebdavlib.path = /usr/lib
}

# QWebDav
android {
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so
    qwebdavlib.files += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so
}
linux:!android {
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so.1
    qwebdavlib.files += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so.1
}
macx {
    LIBS += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.1.dylib
    qwebdavlib.files += $$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.1.dylib
}

# Ubuntu Touch configuration
contains(CONFIG, click) {
    DEFINES += GHOSTCLOUD_UBUNTU_TOUCH
    SOURCES += \
        $$PWD/src/commands/ubuntutouch/utfiledownloadcommandentity.cpp \
        $$PWD/src/settings/db/utaccountsdb.cpp
    HEADERS += \
        $$PWD/src/commands/ubuntutouch/utfiledownloadcommandentity.h \
        $$PWD/src/settings/db/utaccountsdb.h
    LIBS += -lubuntu-download-manager-client -lOnlineAccountsQt
}

INSTALLS += qwebdavlib target
