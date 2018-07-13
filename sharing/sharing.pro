TEMPLATE = lib
TARGET = $$qtLibraryTarget(owncloudshareplugin)
CONFIG += plugin
DEPENDPATH += .

CONFIG += link_pkgconfig
PKGCONFIG += nemotransferengine-qt5

include(../common/common.pri)

# Input
HEADERS += \
    nextcloudconsts.h \
    nextclouduploader.h \
    nextcloudplugininfo.h \
    nextcloudshareplugin.h \
    globaltransfermanager.h \
    globalowncloudbrowser.h

SOURCES += \
    nextcloudplugininfo.cpp \
    nextclouduploader.cpp \
    nextcloudshareplugin.cpp \
    globaltransfermanager.cpp \
    globalowncloudbrowser.cpp

OTHER_FILES += \
    NextcloudShareUI.qml

target.path = /usr/lib/nemo-transferengine/plugins
shareui.files = $$PWD/*.qml
shareui.path = /usr/share/nemo-transferengine/plugins
shareui_icon.files = $$PWD/icons/harbour-owncloud.png
shareui_icon.path = /usr/share/nextcloud-additionals/icons

INSTALLS += target shareui shareui_icon

