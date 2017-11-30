TEMPLATE = lib
TARGET = $$qtLibraryTarget(owncloudshareplugin)
CONFIG += plugin
DEPENDPATH += .

CONFIG += link_pkgconfig
PKGCONFIG += nemotransferengine-qt5

include(../common/common.pri)
QT += qml

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

shareui.files = *.qml
shareui.path = /usr/share/nemo-transferengine/plugins

target.path = /usr/lib/nemo-transferengine/plugins
INSTALLS += target shareui

