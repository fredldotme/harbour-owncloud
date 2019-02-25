import QtQuick 2.0
import QtQuick.Controls 2.4
import QmlUiSet 1.0

Dialog {
    id: dialogRoot

    property int maximumSelections : 1
    property var filesToSelect : [];
    property string remotePath : "";
    property bool externalStorage : true
    property string acceptText : header.defaultAcceptText
    property string cancelText : header.defaultCancelText

    signal errorOccured(string errorString)

    property int __numSelect : 0
    property var __volumes : []

    function indexOfFilePath(path) {
        for(var i = 0; i < filesToSelect.length; i++) {
            var file = filesToSelect[i];
            if (file.path === path)
                return i;
        }
        return -1;
    }

    function isInFilesToSelect(path) {
        return (indexOfFilePath(path) >= 0)
    }

    function refreshVolumes() {
        if (!externalStorage)
            return;

        var newVolumes = fileBrowser.mountedVolumes()
        if ((newVolumes.length) === __volumes.length)
            return

        sdCardMenuItem.visible = (newVolumes.length >= 2)
        usbOtgMenuItem.visible = (newVolumes.length >= 3)
        __volumes = newVolumes
        storageSelection.currentIndex = 0
    }

    Component.onCompleted: {
        refreshVolumes()
    }

    //canAccept: __numSelect > 0;

    LocalFileBrowser {
        id: fileBrowser
        onContentChanged: {
            listView.state = "loadingDone"
        }
        onErrorOccured: {
            listView.state = "loadingDone"
            dialogRoot.errorOccured(error)
        }
    }
    FileDetailsHelper {
        id: fileDetailsHelper
    }

    Flickable {
        anchors.fill: parent

        ComboBox {
            id: storageSelection
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            visible: externalStorage
            currentIndex: 0
            displayText: qsTr("Storage:")
            onAccepted: refreshVolumes()

            model: Menu {
                MenuItem {
                    id: userDirMenuItem
                    text: qsTr("Home directory")
                }
                MenuItem {
                    id: sdCardMenuItem
                    text: qsTr("External storage")
                }
                MenuItem {
                    id: usbOtgMenuItem
                    text: qsTr("External storage (2)")
                }
            }

            onCurrentIndexChanged: {
                console.debug("currentIndex changed: " + currentIndex)
                var currentVolume = __volumes[currentIndex % __volumes.length]
                var newPath = currentVolume.rootPath
                fileBrowser.cd(newPath, newPath)
            }
        }

        ListView {
            id: listView
            clip: true
            anchors.top: storageSelection.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            model: fileBrowser.content
            state: "loadingDone"
            enabled: (state === "loadingDone")
            states: [
                State { name: "loading" },
                State { name: "loadingDone" }
            ]
            transitions: [
                Transition {
                    from: "loading"
                    to: "loadingDone"
                    NumberAnimation {
                        target: listView
                        property: "opacity"
                        duration: 100
                        easing.type: Easing.InOutQuad
                        from: 0.0
                        to: 1.0
                    }
                },
                Transition {
                    from: "loadingDone"
                    to: "loading"
                    NumberAnimation {
                        target: listView
                        property: "opacity"
                        duration: 100
                        easing.type: Easing.InOutQuad
                        from: 1.0
                        to: 0.0
                    }
                }
            ]

            delegate: MouseArea {
                id: delegate

                property var selectedEntry : listView.model[index]

                Rectangle {
                    id: highlighting
                    color: Theme.highlightColor
                    visible: isInFilesToSelect(selectedEntry.path);
                    anchors.fill: parent
                    opacity: 0.5
                }

                Image {
                    id: icon
                    source: selectedEntry.isDirectory ?
                                (selectedEntry.name !== ".." ?
                                    "image://theme/icon-m-folder" :
                                    "image://theme/icon-m-back"
                                 ) :
                                fileDetailsHelper.getIconFromMime(selectedEntry.mimeType)
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.horizontalPageMargin
                    anchors.top: parent.top
                    anchors.topMargin: Theme.paddingSmall
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: Theme.paddingSmall
                    height: label.height
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: label
                    anchors.left: icon.right
                    anchors.leftMargin: Theme.paddingMedium
                    anchors.right: parent.right
                    visible: selectedEntry.name !== ".."
                    text: selectedEntry.name
                    anchors.verticalCenter: parent.verticalCenter
                    color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                }

                onClicked: {
                    var tmpPath = selectedEntry.path;
                    if (selectedEntry.isDirectory) {
                        listView.state = "loading"
                        fileBrowser.cd(tmpPath)
                    } else { // isFile
                        var selectionIndex = indexOfFilePath(tmpPath)
                        if (selectionIndex >= 0) {
                            filesToSelect.splice(selectionIndex, 1);
                            highlighting.visible = false
                        } else if (filesToSelect.length < maximumSelections){
                            filesToSelect.push(selectedEntry);
                            highlighting.visible = true
                        }
                        __numSelect = filesToSelect.length;
                    }
                }
            }
        }
    }
}
