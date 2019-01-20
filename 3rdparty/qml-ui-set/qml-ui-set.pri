CONFIG += qt c++11
QT += quick qml

SOURCES += \
    $$PWD/src/localfilebrowser.cpp \
    $$PWD/src/filedetailshelper.cpp

HEADERS += \
    $$PWD/src/localfilebrowser.h \
    $$PWD/src/filedetailshelper.h \
    $$PWD/src/qml-ui-set.h

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

RESOURCES += \
    $$PWD/qml-ui-set.qrc

lupdate_only {
    OTHER_FILES += \
        $$PWD/ui/FileSelectionDialog.qml \
        $$PWD/ui/TextEntryDialog.qml
}
