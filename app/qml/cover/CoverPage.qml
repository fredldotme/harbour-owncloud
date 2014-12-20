import QtQuick 2.0
import Sailfish.Silica 1.0

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
            visible: transfer.transfering
            running: transfer.transfering
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    CoverActionList {
        enabled: transfer.transfering

        CoverAction {
            iconSource: "image://theme/icon-cover-transfers"
            onTriggered: {
                pageStack.push("TransferPage.qml")
            }
        }
    }
}


