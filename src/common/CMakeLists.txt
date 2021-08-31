# Generated from common.pro.

#####################################################################
## harbourowncloudcommon Generic Library:
#####################################################################

qt_internal_add_cmake_library(harbourowncloudcommon
    SOURCES
        ../../3rdparty/libqtcommandqueue/src/commandentity.cpp ../../3rdparty/libqtcommandqueue/src/commandentity.h
        ../../3rdparty/libqtcommandqueue/src/commandentityinfo.h
        ../../3rdparty/libqtcommandqueue/src/commandqueue.cpp ../../3rdparty/libqtcommandqueue/src/commandqueue.h
        ../../3rdparty/libqtcommandqueue/src/commandunit.cpp ../../3rdparty/libqtcommandqueue/src/commandunit.h
        ../../3rdparty/libqtcommandqueue/src/stdfunctioncommandentity.h
        src/accountworkergenerator.cpp src/accountworkergenerator.h
        src/accountworkers.cpp src/accountworkers.h
        src/auth/authenticationexaminer.cpp src/auth/authenticationexaminer.h
        src/auth/authenticator.cpp src/auth/authenticator.h
        src/auth/flowloginauthenticator.cpp src/auth/flowloginauthenticator.h
        src/auth/qwebdavauthenticator.cpp src/auth/qwebdavauthenticator.h
        src/cacheprovider.cpp src/cacheprovider.h
        src/commands/http/httpcommandentity.cpp src/commands/http/httpcommandentity.h
        src/commands/http/httpgetcommandentity.cpp src/commands/http/httpgetcommandentity.h
        src/commands/nopcommandentity.cpp src/commands/nopcommandentity.h
        src/commands/ocs/ocscommandentity.cpp src/commands/ocs/ocscommandentity.h
        src/commands/ocs/ocssharelistcommandentity.cpp src/commands/ocs/ocssharelistcommandentity.h
        src/commands/ocs/ocsuserinfocommandentity.cpp src/commands/ocs/ocsuserinfocommandentity.h
        src/commands/sync/ncdirtreecommandunit.cpp src/commands/sync/ncdirtreecommandunit.h
        src/commands/sync/ncsynccommandunit.cpp src/commands/sync/ncsynccommandunit.h
        src/commands/webdav/davcopycommandentity.cpp src/commands/webdav/davcopycommandentity.h
        src/commands/webdav/davlistcommandentity.cpp src/commands/webdav/davlistcommandentity.h
        src/commands/webdav/davmovecommandentity.cpp src/commands/webdav/davmovecommandentity.h
        src/commands/webdav/davproppatchcommandentity.cpp src/commands/webdav/davproppatchcommandentity.h
        src/commands/webdav/davrmcommandentity.cpp src/commands/webdav/davrmcommandentity.h
        src/commands/webdav/filedownloadcommandentity.cpp src/commands/webdav/filedownloadcommandentity.h
        src/commands/webdav/fileuploadcommandentity.cpp src/commands/webdav/fileuploadcommandentity.h
        src/commands/webdav/mkdavdircommandentity.cpp src/commands/webdav/mkdavdircommandentity.h
        src/commands/webdav/webdavcommandentity.cpp src/commands/webdav/webdavcommandentity.h
        src/net/abstractfetcher.cpp src/net/abstractfetcher.h
        src/net/avatarfetcher.cpp src/net/avatarfetcher.h
        src/net/thumbnailfetcher.cpp src/net/thumbnailfetcher.h
        src/nextcloudendpointconsts.h
        src/ownclouddbusconsts.h
        src/provider/accountinfo/accountinfoprovider.cpp src/provider/accountinfo/accountinfoprovider.h
        src/provider/accountinfo/ocscommandqueue.cpp src/provider/accountinfo/ocscommandqueue.h
        src/provider/settingsbackedcommandqueue.cpp src/provider/settingsbackedcommandqueue.h
        src/provider/sharing/ocssharingcommandqueue.cpp src/provider/sharing/ocssharingcommandqueue.h
        src/provider/sharing/sharingprovider.cpp src/provider/sharing/sharingprovider.h
        src/provider/storage/cloudstorageprovider.cpp src/provider/storage/cloudstorageprovider.h
        src/provider/storage/webdavcommandqueue.cpp src/provider/storage/webdavcommandqueue.h
        src/settings/db/accountdb.cpp src/settings/db/accountdb.h
        src/settings/db/accountsdbinterface.h
        src/settings/db/syncdb.cpp src/settings/db/syncdb.h
        src/settings/inifilesettings.cpp src/settings/inifilesettings.h
        src/settings/nextcloudsettingsbase.cpp src/settings/nextcloudsettingsbase.h
        src/util/commandutil.cpp src/util/commandutil.h
        src/util/filepathutil.cpp src/util/filepathutil.h
        src/util/providerutils.cpp src/util/providerutils.h
        src/util/qappprepareutil.h
        src/util/shellcommand.cpp src/util/shellcommand.h
        src/util/webdav_utils.cpp src/util/webdav_utils.h
    DEFINES
        QWEBDAVITEM_EXTENDED_PROPERTIES
    INCLUDE_DIRECTORIES
        ../../3rdparty/libqtcommandqueue/src ../../3rdparty/libqtcommandqueue/src
        ../../3rdparty/qwebdavlib/qwebdavlib
        src
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::Network
        Qt::Sql
        Qt::Xml
)

#### Keys ignored in scope 1:.:.:common.pro:<TRUE>:
# DEPENDPATH = "$$PWD/src" "$$PWD/../../3rdparty/qwebdavlib/qwebdavlib" "$$PWD/../../3rdparty/libqtcommandqueue/src" "$$PWD/../../3rdparty/libqtcommandqueue/src"
# INSTALLS = "qwebdavlib" "target"
# OTHER_FILES = "$$PWD/common.pri"
# TEMPLATE = "lib"

## Scopes:
#####################################################################

qt_internal_extend_target(harbourowncloudcommon CONDITION LINUX
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so.1
        Qt::DBus
)

qt_internal_extend_target(harbourowncloudcommon CONDITION ANDROID
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so
        Qt::AndroidExtras
)

qt_internal_extend_target(harbourowncloudcommon CONDITION CMAKE_BUILD_TYPE STREQUAL Release
    DEFINES
        QT_NO_DEBUG_OUTPUT
)

#### Keys ignored in scope 4:.:.:common.pro:(CMAKE_BUILD_TYPE STREQUAL Release):
# QMAKE_POST_LINK = "$(STRIP)" "$(TARGET)"

qt_internal_extend_target(harbourowncloudcommon CONDITION QT___contains___dbus
    SOURCES
        src/settings/permittedsettings.cpp src/settings/permittedsettings.h
)

#### Keys ignored in scope 6:.:.:common.pro:NOT CONFIG___contains___quickcontrols:
# QMAKE_RPATHDIR = "/usr/share/harbour-owncloud/lib"
# qwebdavlib.path = "/usr/share/harbour-owncloud/lib"
# target.path = "/usr/share/harbour-owncloud/lib"

#### Keys ignored in scope 7:.:.:common.pro:CONFIG___contains___quickcontrols:
# qwebdavlib.path = "$$LIBDIR"
# target.path = "$$LIBDIR"

qt_internal_extend_target(harbourowncloudcommon CONDITION MACOS
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.1.dylib
)

#### Keys ignored in scope 10:.:.:common.pro:MACOS:
# qwebdavlib.files = "$$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.1.dylib"

qt_internal_extend_target(harbourowncloudcommon CONDITION CONFIG___contains___click
    SOURCES
        src/commands/ubuntutouch/utfiledownloadcommandentity.cpp src/commands/ubuntutouch/utfiledownloadcommandentity.h
        src/settings/db/utaccountsdb.cpp src/settings/db/utaccountsdb.h
    DEFINES
        GHOSTCLOUD_UBUNTU_TOUCH
    PUBLIC_LIBRARIES
        OnlineAccountsQt
        ubuntu-download-manager-client
)
