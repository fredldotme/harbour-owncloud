import QtQuick 2.0
import Sailfish.Silica 1.0
import "../pages"

CoverBackground {
    anchors.fill: parent

    Column {
        anchors.centerIn: parent

        Image {
            id: icon
            source: "image://theme/harbour-owncloud"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: label
            text: "ownCloud"
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: Theme.fontSizeSmall
        }

        BusyIndicator {
            visible: transfer.transfering || daemonCtrl.uploading
            running: transfer.transfering || daemonCtrl.uploading
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    function findTransfers(page) {
        if(page.objectName === "TransferPage") {
            return true;
        }
    }

    CoverActionList {
        enabled: transfer.transfering || daemonCtrl.uploading

        CoverAction {
            iconSource: "image://theme/icon-cover-transfers"
            onTriggered: {
                if(!pageStack.find(findTransfers)) {
                    pageStack.push("../pages/TransferPage.qml", {}, PageStackAction.Immediate);
                }

                if (!applicationWindow.applicationActive) {
                    applicationWindow.activate()
                }
            }
        }
    }
}


