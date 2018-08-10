import QtQuick 2.0
import Sailfish.Silica 1.0
import SailfishUiSet 1.0

CoverBackground {
    id: coverRoot
    anchors.fill: parent

    Image {
        readonly property int __positioningOffset : 64
        id: icon
        source: "qrc:/icons/icon_gray.svg"
        anchors.top: parent.top
        anchors.topMargin: -__positioningOffset
        anchors.right: parent.right
        anchors.rightMargin: -__positioningOffset
        anchors.left: parent.left
        anchors.leftMargin: __positioningOffset
        anchors.bottom: parent.verticalCenter
        anchors.bottomMargin: __positioningOffset
        fillMode: Image.PreserveAspectFit
        scale: 2
        sourceSize.width: width
        sourceSize.height: height
        visible: true
    }

    Column {
        visible: ocsUserInfo.userInfoEnabled
        anchors.centerIn: coverRoot
        width: coverRoot.width

        CoverDetailItem {
            width: parent.width
            label: qsTr("User:")
            value: ocsUserInfo.hrDisplayName
            visible: value.length > 0
        }
        CoverDetailItem {
            width: parent.width
            label: qsTr("Usage:")
            value: ocsUserInfo.hrUsedBytes
            visible: value.length > 0
        }
        CoverDetailItem {
            width: parent.width
            label: qsTr("Total:")
            value: ocsUserInfo.hrTotalBytes
            visible: value.length > 0
        }
    }

    BusyIndicator {
        anchors.centerIn: coverRoot
        size: BusyIndicatorSize.Large
        visible: isTransfering
        running: isTransfering
    }

    function findTransfers(page) {
        if(page.objectName === "TransferPage") {
            return true
        }
        return false
    }

    CoverActionList {
        enabled: isTransfering

        CoverAction {
            iconSource: "image://theme/icon-cover-transfers"
            onTriggered: {
                if (!pageStack.find(findTransfers)) {
                    pageStack.push(transferPageComponent, {}, PageStackAction.Immediate);
                }

                if (!applicationWindow.applicationActive) {
                    applicationWindow.activate()
                }
            }
        }
    }
}


