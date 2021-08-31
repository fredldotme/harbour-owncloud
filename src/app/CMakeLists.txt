# Generated from app.pro.

#####################################################################
## harbour-owncloud Binary:
#####################################################################

qt_internal_add_executable(harbour-owncloud
    GUI
    SOURCES
        ../qmlcommon/src/qmlmap.cpp ../qmlcommon/src/qmlmap.h
        src/directorycontentmodel.cpp src/directorycontentmodel.h
        src/main.cpp
        src/ocsnetaccessfactory.cpp src/ocsnetaccessfactory.h
        src/webdavmediafeeder.cpp src/webdavmediafeeder.h
    DEFINES
        QWEBDAVITEM_EXTENDED_PROPERTIES QWEBDAVITEM_EXTENDED_PROPERTIES
    INCLUDE_DIRECTORIES
        ../common/../../3rdparty/libqtcommandqueue/src
        ../common/../../3rdparty/qwebdavlib/qwebdavlib
        ../common/src
        ../qmlcommon/src
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Multimedia
        Qt::Network
        Qt::Qml
        Qt::Quick
        Qt::Sql
        Qt::Svg
        Qt::Xml
)

# Resources:
set(icons_resource_files
    "icons/icon_gray.svg"
)

qt_internal_add_resource(harbour-owncloud "icons"
    PREFIX
        "/"
    FILES
        ${icons_resource_files}
)


if(ANDROID)
    set_property(TARGET harbour-owncloud APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
endif()

#### Keys ignored in scope 1:.:.:app.pro:<TRUE>:
# DISTFILES = "$$PWD/android/AndroidManifest.xml" "$$PWD/android/gradle/wrapper/gradle-wrapper.jar" "$$PWD/android/gradlew" "$$PWD/android/res/values/libs.xml" "$$PWD/android/build.gradle" "$$PWD/android/gradle/wrapper/gradle-wrapper.properties" "$$PWD/android/gradlew.bat" "android/src/me/fredl/ghostcloud/ShareUtil.java"
# OTHER_FILES = "translations/*.ts" "harbour-owncloud.desktop"
# SAILFISHAPP_ICONS = "86x86" "108x108" "128x128" "172x172"
# TRANSLATIONS = "translations/harbour-owncloud-de.ts" "translations/harbour-owncloud-fi.ts" "translations/harbour-owncloud-fr.ts" "translations/harbour-owncloud-sl.ts" "translations/harbour-owncloud-sv.ts" "translations/harbour-owncloud-zh_CN.ts"

## Scopes:
#####################################################################

qt_internal_extend_target(harbour-owncloud CONDITION NOT CONFIG___contains___quickcontrols
    SOURCES
        ../../3rdparty/sailfish-ui-set/src/filedetailshelper.cpp ../../3rdparty/sailfish-ui-set/src/filedetailshelper.h
        ../../3rdparty/sailfish-ui-set/src/localfilebrowser.cpp ../../3rdparty/sailfish-ui-set/src/localfilebrowser.h
        ../../3rdparty/sailfish-ui-set/src/sailfish-ui-set.h
    INCLUDE_DIRECTORIES
        ../../3rdparty/sailfish-ui-set/src
    PUBLIC_LIBRARIES
        Qt::Qml
        Qt::Quick
)

if(NOT CONFIG___contains___quickcontrols)
    # Resources:
    set(qml-sfos_resource_files
        "qml/sfos/OcsUserInfo.qml"
        "qml/sfos/cover/CoverDetailItem.qml"
        "qml/sfos/cover/CoverPage.qml"
        "qml/sfos/harbour-owncloud.qml"
        "qml/sfos/js/TransferUtil.js"
        "qml/sfos/navigation/BrowserCommandPageFlow.qml"
        "qml/sfos/navigation/CommandPageFlow.qml"
        "qml/sfos/pages/About.qml"
        "qml/sfos/pages/SettingsPage.qml"
        "qml/sfos/pages/TransferPage.qml"
        "qml/sfos/pages/browser/FileBrowser.qml"
        "qml/sfos/pages/browser/FileDetails.qml"
        "qml/sfos/pages/browser/RemoteDirSelectDialog.qml"
        "qml/sfos/pages/browser/controls/FileOperationsContextMenu.qml"
        "qml/sfos/pages/login/AccountSelection.qml"
        "qml/sfos/pages/login/BasicAuthentication.qml"
        "qml/sfos/pages/login/Entrance.qml"
        "qml/sfos/pages/login/FlowAuthentication.qml"
        "qml/sfos/pages/login/SSLErrorDialog.qml"
    )

    qt_internal_add_resource(harbour-owncloud "qml-sfos"
        PREFIX
            "/"
        FILES
            ${qml-sfos_resource_files}
    )
    set_source_files_properties("../../3rdparty/sailfish-ui-set/ui/AbortableBusyIndicator.qml"
        PROPERTIES QT_RESOURCE_ALIAS "AbortableBusyIndicator.qml"
    )
    set_source_files_properties("../../3rdparty/sailfish-ui-set/ui/CircularImageButton.qml"
        PROPERTIES QT_RESOURCE_ALIAS "CircularImageButton.qml"
    )
    set_source_files_properties("../../3rdparty/sailfish-ui-set/ui/FileSelectionDialog.qml"
        PROPERTIES QT_RESOURCE_ALIAS "FileSelectionDialog.qml"
    )
    set_source_files_properties("../../3rdparty/sailfish-ui-set/ui/HidingPanel.qml"
        PROPERTIES QT_RESOURCE_ALIAS "HidingPanel.qml"
    )
    set_source_files_properties("../../3rdparty/sailfish-ui-set/ui/TextEntryDialog.qml"
        PROPERTIES QT_RESOURCE_ALIAS "TextEntryDialog.qml"
    )
    set(sailfish-ui-set_resource_files
        "../../3rdparty/sailfish-ui-set/ui/AbortableBusyIndicator.qml"
        "../../3rdparty/sailfish-ui-set/ui/CircularImageButton.qml"
        "../../3rdparty/sailfish-ui-set/ui/FileSelectionDialog.qml"
        "../../3rdparty/sailfish-ui-set/ui/HidingPanel.qml"
        "../../3rdparty/sailfish-ui-set/ui/TextEntryDialog.qml"
    )

    qt_internal_add_resource(harbour-owncloud "sailfish-ui-set"
        PREFIX
            "/sailfish-ui-set"
        BASE
            "../../3rdparty/sailfish-ui-set"
        FILES
            ${sailfish-ui-set_resource_files}
    )
endif()

#### Keys ignored in scope 2:.:.:app.pro:NOT CONFIG___contains___quickcontrols:
# QMAKE_RPATHDIR = "/usr/share/harbour-owncloud/lib"

#### Keys ignored in scope 14:.:../../3rdparty/sailfish-ui-set:../../3rdparty/sailfish-ui-set/sailfish-ui-set.pri:lupdate_only:
# OTHER_FILES = "$$PWD/ui/FileSelectionDialog.qml" "$$PWD/ui/TextEntryDialog.qml"

qt_internal_extend_target(harbour-owncloud CONDITION CONFIG___contains___quickcontrols
    SOURCES
        ../../3rdparty/qml-ui-set/src/filedetailshelper.cpp ../../3rdparty/qml-ui-set/src/filedetailshelper.h
        ../../3rdparty/qml-ui-set/src/localfilebrowser.cpp ../../3rdparty/qml-ui-set/src/localfilebrowser.h
        ../../3rdparty/qml-ui-set/src/qml-ui-set.h
    DEFINES
        GHOSTCLOUD_UI_QUICKCONTROLS GHOSTCLOUD_UI_QUICKCONTROLS
    INCLUDE_DIRECTORIES
        ../../3rdparty/qml-ui-set/src
    PUBLIC_LIBRARIES
        Qt::Qml
        Qt::Quick
)

if(CONFIG___contains___quickcontrols)
    # Resources:
    set(qml_resource_files
        "qml/qqc/BrowserCommandPageFlow.qml"
        "qml/qqc/CommandPageFlow.qml"
        "qml/qqc/controls/GCButton.qml"
        "qml/qqc/dialogs/RemoteDirSelectDialog.qml"
        "qml/qqc/dialogs/SSLErrorDialog.qml"
        "qml/qqc/dialogs/WebDavAccountDialog.qml"
        "qml/qqc/dialogs/fileselect/QQDFilePicker.qml"
        "qml/qqc/dialogs/fileselect/UbuntuContentHubOpener.qml"
        "qml/qqc/dialogs/fileselect/UbuntuContentPicker.qml"
        "qml/qqc/main.qml"
        "qml/qqc/pages/About.qml"
        "qml/qqc/pages/AccountSelection.qml"
        "qml/qqc/pages/SettingsPage.qml"
        "qml/qqc/pages/TransferPage.qml"
        "qml/qqc/pages/browser/FileBrowser.qml"
        "qml/qqc/pages/browser/FileDetails.qml"
    )

    qt_internal_add_resource(harbour-owncloud "qml"
        PREFIX
            "/"
        FILES
            ${qml_resource_files}
    )
    set(theme_resource_files
        "icons/theme/actions/24/list-add.svg"
        "icons/theme/actions/32/application-menu.svg"
        "icons/theme/actions/32/document-new.svg"
        "icons/theme/actions/32/folder-new.svg"
        "icons/theme/actions/32/go-down.svg"
        "icons/theme/actions/32/go-previous.svg"
        "icons/theme/actions/32/go-up.svg"
        "icons/theme/actions/32/media-playback-pause.svg"
        "icons/theme/actions/32/media-playback-start.svg"
        "icons/theme/actions/32/view-refresh.svg"
        "icons/theme/actions/symbolic/window-close-symbolic.svg"
        "icons/theme/index.theme"
        "icons/theme/mimetypes/64/audio-x-generic.svg"
        "icons/theme/mimetypes/64/image-x-generic.svg"
        "icons/theme/mimetypes/64/text-x-plain.svg"
        "icons/theme/mimetypes/64/video-x-generic.svg"
        "icons/theme/places/64/folder.svg"
        "qtquickcontrols2.conf"
    )

    qt_internal_add_resource(harbour-owncloud "theme"
        PREFIX
            "/"
        FILES
            ${theme_resource_files}
    )
    set_source_files_properties("../../3rdparty/qml-ui-set/ui/AbortableBusyIndicator.qml"
        PROPERTIES QT_RESOURCE_ALIAS "AbortableBusyIndicator.qml"
    )
    set_source_files_properties("../../3rdparty/qml-ui-set/ui/CircularImageButton.qml"
        PROPERTIES QT_RESOURCE_ALIAS "CircularImageButton.qml"
    )
    set_source_files_properties("../../3rdparty/qml-ui-set/ui/DetailItem.qml"
        PROPERTIES QT_RESOURCE_ALIAS "DetailItem.qml"
    )
    set_source_files_properties("../../3rdparty/qml-ui-set/ui/FileSelectionDialog.qml"
        PROPERTIES QT_RESOURCE_ALIAS "FileSelectionDialog.qml"
    )
    set_source_files_properties("../../3rdparty/qml-ui-set/ui/HidingPanel.qml"
        PROPERTIES QT_RESOURCE_ALIAS "HidingPanel.qml"
    )
    set_source_files_properties("../../3rdparty/qml-ui-set/ui/TextEntryDialog.qml"
        PROPERTIES QT_RESOURCE_ALIAS "TextEntryDialog.qml"
    )
    set(qml-ui-set_resource_files
        "../../3rdparty/qml-ui-set/ui/AbortableBusyIndicator.qml"
        "../../3rdparty/qml-ui-set/ui/CircularImageButton.qml"
        "../../3rdparty/qml-ui-set/ui/DetailItem.qml"
        "../../3rdparty/qml-ui-set/ui/FileSelectionDialog.qml"
        "../../3rdparty/qml-ui-set/ui/HidingPanel.qml"
        "../../3rdparty/qml-ui-set/ui/TextEntryDialog.qml"
    )

    qt_internal_add_resource(harbour-owncloud "qml-ui-set"
        PREFIX
            "/qml-ui-set"
        BASE
            "../../3rdparty/qml-ui-set"
        FILES
            ${qml-ui-set_resource_files}
    )
    set_source_files_properties("../../3rdparty/qml-ui-set/img/close.png"
        PROPERTIES QT_RESOURCE_ALIAS "close.png"
    )
    set(qml-ui-set1_resource_files
        "../../3rdparty/qml-ui-set/img/close.png"
    )

    qt_internal_add_resource(harbour-owncloud "qml-ui-set1"
        PREFIX
            "/qml-ui-set-img"
        BASE
            "../../3rdparty/qml-ui-set"
        FILES
            ${qml-ui-set1_resource_files}
    )
endif()

#### Keys ignored in scope 3:.:.:app.pro:CONFIG___contains___quickcontrols:
# TEMPLATE = "app"

#### Keys ignored in scope 4:.:.:app.pro:UNIX AND NOT MACOS:
# INSTALLS = "desktop" "target"
# QMAKE_RPATHDIR = "$$OUT_PWD/../../3rdparty/qwebdav" "$$OUT_PWD/../common"
# desktop.files = "harbour-owncloud.desktop"
# desktop.path = "/usr/share/applications"
# target.path = "/usr/bin"

#### Keys ignored in scope 5:.:.:app.pro:IOS:
# QMAKE_INFO_PLIST = "$$PWD/ios/Info.plist"

#### Keys ignored in scope 6:.:.:app.pro:MACOS:
# QMAKE_BUNDLE_DATA = "info"
# info.files = "$$PWD/macos/Info.plist"
# info.path = "Contents"

qt_internal_extend_target(harbour-owncloud CONDITION ANDROID AND CONFIG___contains___quickcontrols
    PUBLIC_LIBRARIES
        # Remove: L
        ${CMAKE_CURRENT_BINARY_DIR}/../../3rdparty/openssl
        Qt::AndroidExtras
        crypto
        ssl
)

#### Keys ignored in scope 7:.:.:app.pro:ANDROID:
# ANDROID_EXTRA_LIBS = "$$OUT_PWD/../../3rdparty/openssl/libcrypto.so" "$$OUT_PWD/../../3rdparty/openssl/libssl.so"

#### Keys ignored in scope 16:.:../../3rdparty/qml-ui-set:../../3rdparty/qml-ui-set/qml-ui-set.pri:lupdate_only:
# OTHER_FILES = "$$PWD/ui/FileSelectionDialog.qml" "$$PWD/ui/TextEntryDialog.qml"

qt_internal_extend_target(harbour-owncloud CONDITION NOT QT___contains___dbus
    SOURCES
        src/daemonctrl/dummydaemonctrl.cpp src/daemonctrl/dummydaemonctrl.h
)

qt_internal_extend_target(harbour-owncloud CONDITION QT___contains___dbus
    SOURCES
        src/daemonctrl/daemoncontrol.cpp src/daemonctrl/daemoncontrol.h
)

qt_internal_extend_target(harbour-owncloud CONDITION NOT QT___contains___androidextras
    SOURCES
        src/os/dummy/dummyintentfileselector.cpp src/os/dummy/dummyintentfileselector.h
)

qt_internal_extend_target(harbour-owncloud CONDITION QT___contains___androidextras
    SOURCES
        src/os/android/intentfileselector.cpp src/os/android/intentfileselector.h
        src/os/android/intentfileselectorreceiver.cpp src/os/android/intentfileselectorreceiver.h
)

#### Keys ignored in scope 12:.:.:app.pro:lupdate_only:
# OTHER_FILES = "qml/harbour-owncloud.qml" "qml/cover/CoverPage.qml" "qml/pages/FileBrowser.qml" "qml/pages/FileDetails.qml" "qml/pages/Login.qml" "qml/pages/SSLErrorDialog.qml" "qml/pages/SettingsPage.qml" "qml/pages/TransferPage.qml" "qml/pages/RemoteDirSelectDialog.qml" "qml/dummy/FileSelectionDialog.qml"

qt_internal_extend_target(harbour-owncloud CONDITION LINUX
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so.1
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../src/common/libharbourowncloudcommon.so.1
)

qt_internal_extend_target(harbour-owncloud CONDITION LINUX AND NOT CONFIG___contains___quickcontrols
    PUBLIC_LIBRARIES
        Qt::DBus
)

qt_internal_extend_target(harbour-owncloud CONDITION ANDROID
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../3rdparty/openssl/libcrypto.so
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../3rdparty/openssl/libssl.so
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../src/common/libharbourowncloudcommon.so
        Qt::AndroidExtras
)

qt_internal_extend_target(harbour-owncloud CONDITION CMAKE_BUILD_TYPE STREQUAL Release
    DEFINES
        QT_NO_DEBUG_OUTPUT
)

#### Keys ignored in scope 21:.:../common:../common/common.pri:(CMAKE_BUILD_TYPE STREQUAL Release):
# QMAKE_POST_LINK = "$(STRIP)" "$(TARGET)"

qt_internal_extend_target(harbour-owncloud CONDITION MACOS
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.1.dylib
)

#### Keys ignored in scope 25:.:../common:../common/common.pri:MACOS:
# PRE_TARGETDEPS = "$$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.1.dylib"
# QMAKE_BUNDLE_DATA = "libs"
# libs.files = "$$OUT_PWD/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.1.dylib"
# libs.path = "Contents/MacOS"

qt_internal_extend_target(harbour-owncloud CONDITION CMAKE_BUILD_TYPE STREQUAL Release AND MACOS
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../src/common/libharbourowncloudcommon.1.dylib
)

#### Keys ignored in scope 26:.:../common:../common/common.pri:(CMAKE_BUILD_TYPE STREQUAL Release):
# PRE_TARGETDEPS = "$$OUT_PWD/../../src/common/libharbourowncloudcommon.1.dylib"
# libs.files = "$$OUT_PWD/../../src/common/libharbourowncloudcommon.1.dylib"

qt_internal_extend_target(harbour-owncloud CONDITION CMAKE_BUILD_TYPE STREQUAL Debug AND MACOS
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../src/common/libharbourowncloudcommon_debug.1.dylib
)

#### Keys ignored in scope 27:.:../common:../common/common.pri:(CMAKE_BUILD_TYPE STREQUAL Debug):
# PRE_TARGETDEPS = "$$OUT_PWD/../../src/common/libharbourowncloudcommon_debug.1.dylib"
# libs.files = "$$OUT_PWD/../../src/common/libharbourowncloudcommon_debug.1.dylib"

qt_internal_extend_target(harbour-owncloud CONDITION IOS
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.a
)

qt_internal_extend_target(harbour-owncloud CONDITION CMAKE_BUILD_TYPE STREQUAL Release AND IOS
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../src/common/libharbourowncloudcommon.a
)

qt_internal_extend_target(harbour-owncloud CONDITION CMAKE_BUILD_TYPE STREQUAL Debug AND IOS
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../src/common/libharbourowncloudcommon_debug.a
)

qt_internal_extend_target(harbour-owncloud CONDITION CONFIG___contains___sailfish_build
    PUBLIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../3rdparty/qwebdavlib/qwebdavlib/libqwebdav.so.1
        ${CMAKE_CURRENT_BINARY_DIR}/../common/../../src/common/libharbourowncloudcommon.so.1
)

qt_internal_extend_target(harbour-owncloud CONDITION CONFIG___contains___click
    DEFINES
        GHOSTCLOUD_UBUNTU_TOUCH
        OS_SUPPORTS_THEME_PROVIDER
)
