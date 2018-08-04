TEMPLATE = lib
TARGET = $$qtLibraryTarget(harbourowncloudqmlplugin)
CONFIG += plugin
DEPENDPATH += .

include($$PWD/../common/common.pri)
include($$PWD/../qmlcommon/qmlcommon.pri)

SOURCES += \
    harbourowncloudqmlplugin.cpp

target.path = $$[QT_INSTALL_QML]/com/github/beidl/harbourowncloud
qmldir.files = $$PWD/qmldir
qmldir.path = $$[QT_INSTALL_QML]/com/github/beidl/harbourowncloud

INSTALLS += target qmldir

HEADERS += \
    harbourowncloudqmlplugin.h
