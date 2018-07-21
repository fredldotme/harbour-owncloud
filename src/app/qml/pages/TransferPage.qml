import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Page {
    id: pageRoot
    objectName: "TransferPage"

    Component.onCompleted: {
        if(daemonCtrl.uploading) {
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
            if(daemonCtrl.uploading) {
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
            enabled: (transfers.count < 1) && !daemonCtrl.uploading
        }

        SilicaListView {
            id: listView
            anchors.fill: parent
            model: transfers

            delegate: ListItem {
                id: delegate
                property CommandEntity commandEntity : listView.model[index]

                Image {
                    id: transferTypeIcon
                    x: 16
                    height: parent.height
                    source: commandEntity.info().property("type") === "fileDownload"?
                                "image://theme/icon-m-download" :
                                "image://theme/icon-m-upload"
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: label
                    text: commandEntity.info().property("fileName")
                    anchors.top: parent.top
                    anchors.left: transferTypeIcon.right
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                }

                ProgressBar {
                    id: transferProgress
                    anchors.top: label.bottom
                    width: parent.width
                    anchors.leftMargin: Theme.paddingLarge
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
