import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Page {
    id: pageRoot

    Component.onCompleted: {
        listView.model = transfer.getTransfers();
        if(daemonCtrl.uploading) {
            daemonAnimationOut.stop()
            daemonAnimationIn.start()
        } else {
            daemonAnimationIn.stop()
            daemonAnimationOut.start()
        }
    }

    Connections {
        target: transfer
        onTransferAdded: {
            listView.model = transfer.getTransfers();
        }
    }

    Connections {
        target: transfer
        onTransferingChanged: {
            listView.model = transfer.getTransfers();
        }
    }

    property Item transferContextMenu;
    property TransferEntry selectedEntry;
    property BackgroundItem selectedItem;
    property RemorseItem selectedRemorse;

    PageHeader {
        id: header
        title: "File transfers"
    }

    BackgroundItem {
        id: daemonProgress
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        visible: daemonCtrl.daemonInstalled
        height: visible ? 78 * opacity : 0
        anchors.horizontalCenter: parent.horizontalCenter
        BusyIndicator {
            id: indicator
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingLarge
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.height
            running: parent.visible
        }
        Label {
            id: backupText
            text: "Camera backup in progress..."
            anchors.left: indicator.right
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
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
        anchors.top: daemonProgress.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        SilicaListView {
            id: listView
            anchors.fill: parent
            //header:

            delegate: BackgroundItem {
                id: delegate
                property bool menuOpen: transferContextMenu != null && transferContextMenu.parent === delegate
                height: menuOpen ? transferContextMenu.height + bgItem.height : bgItem.height

                BackgroundItem {
                    id: bgItem

                    RemorseItem { id: remorseItem }

                    Image {
                        id: transferTypeIcon
                        x: 16
                        height: label.height + transferProgress.height
                        source: listView.model[index].direction === 0 ?
                                    "image://theme/icon-m-download" :
                                    "image://theme/icon-m-upload"
                        fillMode: Image.PreserveAspectFit
                    }

                    Label {
                        id: label
                        text: listView.model[index].name
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
                    }

                    onPressAndHold: {
                        selectedEntry = listView.model[index];
                        selectedRemorse = remorseItem;
                        selectedItem = delegate
                        if (!transferContextMenu)
                            transferContextMenu = contextMenuComponent.createObject(listView)
                        transferContextMenu.show(delegate)
                    }

                    Connections {
                        /*
                          Use a connection instead of directly assigning model[index].value
                          The TransferEntry object appears to get deleted between emitting the
                          signal and trying to read the property. Use this connection instead.
                          */
                        target: listView.model[index]
                        onProgressChanged: {
                            transferProgress.value = progress
                        }
                    }
                }
            }
            VerticalScrollDecorator {}

            Label {
                id: noTransfersHint
                anchors.centerIn: parent
                text: qsTr("No file transfers pending")
                visible: (listView.model === undefined || listView.model.length === 0) && !daemonCtrl.uploading
            }

            Component {
                id: contextMenuComponent
                ContextMenu {
                    onClosed: {
                        selectedEntry = null
                        selectedItem = null
                    }

                    MenuItem {
                        text: "Cancel"

                        property TransferEntry tmpEntry;

                        onClicked: {
                            tmpEntry = selectedEntry
                            remorseItem.execute(selectedItem, "Canceling transfer", function() {
                                tmpEntry.cancelTransfer();
                                selectedRemorse = null;
                            })
                        }
                    }
                }
            }
        }
    }
}
