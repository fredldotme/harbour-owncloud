import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Page {
    id: pageRoot
    objectName: "TransferPage"

    Component.onCompleted: {
        if (daemonCtrl.uploading) {
            daemonAnimationOut.stop();
            daemonAnimationIn.start();
        } else {
            daemonAnimationIn.stop();
            daemonAnimationOut.start();
        }
    }

    property CommandEntity selectedEntry : null;

    PageHeader {
        id: header
        title: qsTr("File transfers")
    }

    BackgroundItem {
        id: daemonProgress
        anchors.top: header.bottom
        width: parent.width
        visible: daemonCtrl.daemonInstalled
        height: visible ? 78 * opacity : 0
        anchors.horizontalCenter: parent.horizontalCenter
        BusyIndicator {
            id: indicator
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingLarge + 39 - (parent.height/2)
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.height
            running: parent.visible
        }
        Label {
            id: backupText
            text: qsTr("Camera backup in progress...")
            anchors.left: indicator.right
            anchors.leftMargin: 16 + 39 - (parent.height/2)
            anchors.verticalCenter: indicator.verticalCenter
        }
    }

    Connections {
        target: daemonCtrl
        onUploadingChanged: {
            if (daemonCtrl.uploading) {
                daemonAnimationOut.stop()
                daemonAnimationIn.start()
            } else {
                daemonAnimationIn.stop()
                daemonAnimationOut.start()
            }
        }
    }

    NumberAnimation {
        id: daemonAnimationIn
        target: daemonProgress;
        property: "opacity";
        duration: 150;
        from: 0.0
        to: 1.0
        easing.type: Easing.InOutQuad
        running: false
    }

    NumberAnimation {
        id: daemonAnimationOut
        target: daemonProgress;
        property: "opacity";
        duration: 150;
        from: 1.0
        to: 0.0
        easing.type: Easing.InOutQuad
        running: false
    }

    SilicaFlickable {
        id: flickableRoot
        anchors.top: daemonProgress.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        ViewPlaceholder {
            id: noTransfersHint
            anchors.centerIn: parent
            text: qsTr("No pending file transfers")
            enabled: (transferQueue.queue.length < 1) && !daemonCtrl.uploading
        }

        SilicaListView {
            id: listView
            anchors.fill: parent
            spacing: Theme.paddingMedium
            model: transferQueue.queue

            delegate: ListItem {
                id: delegate

                property CommandEntity commandEntity : listView.model[index]

                property string commandType :
                    commandEntity.info.property("type") !== undefined ?
                        commandEntity.info.property("type") : ""

                property string fileName :
                    commandEntity.info.property("fileName") !== undefined ?
                        commandEntity.info.property("fileName") : ""

                function getIconForCommandType() {
                    if (commandType == "fileDownload")
                        return "image://theme/icon-m-download"
                    if (commandType == "fileUpload")
                        return "image://theme/icon-m-upload"
                    if (commandType == "fileOpen")
                        return "image://theme/icon-m-file-other"
                    return ""
                }

                // The transfer queue could contain additional entities, including prop patch requests.
                // Hide all except for downloads, uploads and file opening requests.
                property bool isTransfer :
                    (commandType === "fileDownload" ||
                     commandType === "fileUpload")

                readonly property bool isFileOpen :
                    (commandType === "fileOpen")

                readonly property string openFileHint :
                    qsTr("Open: '%1'").arg(fileName)

                property bool isAllowedVisible :
                    (isTransfer || isFileOpen)

                Component.onCompleted: {
                    if (!isAllowedVisible) height = 0
                }
                visible: isAllowedVisible

                Image {
                    id: transferTypeIcon
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: 32
                    anchors.bottom: parent.bottom
                    height: parent.height
                    width: height
                    source: getIconForCommandType()
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: label
                    text: !isFileOpen ? fileName : openFileHint
                    anchors.top: parent.top
                    anchors.left: transferTypeIcon.right
                    anchors.leftMargin: 32
                    anchors.right: parent.right
                    color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                }

                ProgressBar {
                    id: transferProgress
                    anchors.top: label.bottom
                    anchors.left: transferTypeIcon.left
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingLarge
                    anchors.bottom: parent.bottom
                    visible: isTransfer
                    minimumValue: 0.0
                    maximumValue: 1.0
                    value: commandEntity.progress
                }

                onPressAndHold: {
                    selectedEntry = commandEntity;
                    if (!menu)
                        menu = contextMenuComponent.createObject(listView)
                    openMenu()
                }
            }
            VerticalScrollDecorator {}

            Component {
                id: contextMenuComponent
                ContextMenu {
                    onClosed: {
                        selectedEntry = null
                    }

                    MenuItem {
                        text: qsTr("Cancel")
                        onClicked: {
                            selectedEntry.abort()
                        }
                    }
                }
            }
        }
    }
}
