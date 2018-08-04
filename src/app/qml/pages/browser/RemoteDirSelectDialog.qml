import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Dialog {
    id: dialogRoot
    canAccept: !isLoadingDirectory

    property string remotePath : "/"
    property bool isLoadingDirectory : true

    // Prepend ".." in case remotePath !== "/"
    function mangledDirectoryList(dirs) {
        if (remotePath === "/")
            return dirs;
        return [{ name: "..", isDirectory: true }].concat(dirs)
    }

    function getDirectoryContent(path) {
        remotePath = FilePathUtil.getCanonicalPath(path)

        if (directoryContents.contains(remotePath)) {
            listView.model = mangledDirectoryList(directoryContents.value(remotePath))
            isLoadingDirectory = false;
            return;
        }
        browserCommandQueue.directoryListingRequest(remotePath, true)
    }

    Component.onCompleted: {
        getDirectoryContent(remotePath)
    }

    Connections {
        target: directoryContents
        onInserted: {
            console.log("key:" + key + " remotePath:" + remotePath)
            if (key !== remotePath)
                return;

            listView.model = mangledDirectoryList(directoryContents.value(key))
            isLoadingDirectory = false;
        }
    }

    SilicaFlickable {
        clip: true

        anchors.top: dialogHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        BusyIndicator {
            anchors.centerIn: parent
            running: isLoadingDirectory
        }

        SilicaListView {
            id: listView
            anchors.fill: parent
            enabled: !isLoadingDirectory

            delegate: ListItem {
                property var davInfo : listView.model[index]
                id: bgItem
                visible: davInfo.isDirectory

                Image {
                    id: icon
                    source: davInfo.name !== ".." ?
                                "image://theme/icon-m-folder" :
                                "image://theme/icon-m-back"
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
                    text: davInfo.name !== ".." ? davInfo.name : ""
                    anchors.verticalCenter: parent.verticalCenter
                    color: bgItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                    truncationMode: TruncationMode.Fade
                }

                onClicked: {
                    if(!davInfo.isDirectory)
                        return;

                    var newTargetDir = FilePathUtil.getCanonicalPath(remotePath + "/" +
                                                                     davInfo.name + "/")
                    console.log(newTargetDir)
                    isLoadingDirectory = true;
                    getDirectoryContent(newTargetDir)
                }
            }
        }
    }


    DialogHeader {
        id: dialogHeader
    }
}

