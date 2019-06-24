import QtQuick 2.0
import harbour.owncloud 1.0
import SailfishUiSet 1.0

Item {
    property CommandQueue commandQueue : null

    property bool userInfoEnabled : false
    property string displayName : ""
    property string emailAddress : ""
    property string usedBytes : ""
    property string freeBytes : ""
    property string totalBytes : ""

    readonly property string hrDisplayName :
        displayName.length < 20 ?
            displayName :
            displayName.substring(0, 20) + ".."
    readonly property string hrMailAddress :
        emailAddress.length < 20 ?
            emailAddress :
            emailAddress.substring(0, 20) + ".."
    readonly property string hrUsedBytes :
        usedBytes.length < 15 ?
            usedBytes :
            usedBytes.substring(0, 15) + ".."
    readonly property string hrFreeBytes :
        freeBytes.length < 15 ?
            freeBytes :
            freeBytes.substring(0, 15) + ".."
    readonly property string hrTotalBytes :
        totalBytes.length < 15 ?
            totalBytes :
            totalBytes.substring(0, 15) + ".."

    function reset() {
        userInfoEnabled = false
        displayName = ""
        emailAddress = ""
        usedBytes = ""
        totalBytes = ""
    }

    FileDetailsHelper {
        id: fileDetailsHelper
    }

    Connections {
        target: commandQueue
        onCommandFinished: {
            var ocsType = receipt.info.property("type")
            if (ocsType !== "userInfo")
                return;

            if (receipt.result === undefined)
                return;
            if (receipt.result.enabled === undefined)
                return;

            ocsUserInfo.userInfoEnabled = receipt.result.enabled;
            ocsUserInfo.displayName = receipt.result.displayName;
            ocsUserInfo.emailAddress = receipt.result.email;
            ocsUserInfo.usedBytes = fileDetailsHelper.getHRSizeFromString(
                        receipt.result.usedBytes);
            ocsUserInfo.freeBytes = fileDetailsHelper.getHRSizeFromString(
                        receipt.result.freeBytes);
            ocsUserInfo.totalBytes = fileDetailsHelper.getHRSizeFromString(
                        receipt.result.totalBytes);
        }
    }
}
