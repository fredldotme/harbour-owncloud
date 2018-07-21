import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Dialog {
    id: dialogRoot
    canAccept: !isLoadingDirectory

    property string remotePath : "/"
    property bool isLoadingDirectory : true

    Component.onCompleted: {
        if (directoryContents.contains(remotePath)) {
            listView.model = directoryContents.value(remotePath)
            isLoadingDirectory = false;
            return;
        }

        var command = browser.directoryListingRequest(remotePath)
        transfer.mainQueue.enqueue(command)
        transfer.mainQueue.run()
    }

    Connections {
        target: directoryContents
        onInserted: {
            console.log("key:" + key + " remotePath:" + remotePath)
            if (key !== remotePath)
                return;

            listView.model = directoryContents.value(key)
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

            property var davInfo : listView.model[index]

            delegate: ListItem {
                id: bgItem
                Image {
                    id: icon
                    source: davInfo.name !== ".." ?
                                "image://theme/icon-m-folder" :
                                "image://theme/icon-m-back"
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.top: parent.top
                    anchors.topMargin: 18
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 18
                    height: label.height
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: label
                    x: icon.x + icon.width + 12
                    y: icon.y - icon.height + 6
                    text: davInfo.name
                    anchors.verticalCenter: parent.verticalCenter
                    color: bgItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                }

                onClicked: {
                    if(davInfo.isDirectory) {
                        var newTargetDir = browser.getCanonicalPath(remotePath + davInfo.name + "/");
                        var command = browser.directoryListingRequest(newTargetDir);
                        transfer.mainQueue.enqueue(command)
                        transfer.mainQueue.run()
                        isLoadingDirectory = true;
                    }
                }
            }
        }
    }


    DialogHeader {
        id: dialogHeader
    }
}

