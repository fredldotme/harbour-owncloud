import QtQuick 2.0
import Sailfish.Silica 1.0
import SailfishUiSet 1.0

CoverBackground {
    id: coverRoot
    anchors.fill: parent

    property bool userInfoEnabled : false
    property string displayName : ""
    property string email : ""
    property string usedBytes : ""
    property string totalBytes : ""

    readonly property string hrDisplayName :
        displayName.length < 20 ?
            displayName :
            displayName.substring(0, 20) + ".."
    readonly property string hrMailAddress :
        email.length < 20 ?
            email :
            email.substring(0, 20) + ".."
    readonly property string hrUsedBytes :
        usedBytes.length < 15 ?
            usedBytes :
            usedBytes.substring(0, 15) + ".."
    readonly property string hrTotalBytes :
        totalBytes.length < 15 ?
            totalBytes :
            totalBytes.substring(0, 15) + ".."


    FileDetailsHelper {
        id: fileDetailsHelper
    }

    Connections {
        target: ocsCommandQueue
        onCommandFinished: {
            var ocsType = receipt.info.property("type")
            if (ocsType !== "userInfo")
                return;

            if (receipt.result.enabled === undefined)
                return;

            userInfoEnabled = receipt.result.enabled;
            displayName = receipt.result.displayName;
            email = receipt.result.email;
            usedBytes = fileDetailsHelper.getHRSizeFromString(
                        receipt.result.usedBytes);
            totalBytes = fileDetailsHelper.getHRSizeFromString(
                        receipt.result.totalBytes);
        }
    }

    Image {
        id: icon
        source: "qrc:/icons/icon_gray.svg"
        anchors.top: parent.top
        anchors.topMargin: -32
        anchors.right: parent.right
        anchors.rightMargin: -32
        anchors.left: parent.left
        anchors.leftMargin: parent.width/4
        height: width
        scale: 2
        sourceSize.width: width
        sourceSize.height: height
    }

    Column {
        visible: userInfoEnabled
        anchors.centerIn: coverRoot
        width: coverRoot.width

        CoverDetailItem {
            width: parent.width
            label: qsTr("User:")
            value: hrDisplayName
            visible: value.length > 0
        }
        /*CoverDetailItem {
            width: parent.width
            label: qsTr("Mail:")
            value: hrMailAddress
            visible: value.length > 0
        }*/
        CoverDetailItem {
            width: parent.width
            label: qsTr("Usage:")
            value: hrUsedBytes
            visible: value.length > 0
        }
        CoverDetailItem {
            width: parent.width
            label: qsTr("Total:")
            value: hrTotalBytes
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


