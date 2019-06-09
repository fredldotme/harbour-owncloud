import QtQuick 2.0
import QtQuick.Controls 2.2

Dialog {
    id: textEntryDialog
    standardButtons: Dialog.Ok | Dialog.Cancel

    property string md5Fingerprint : ""
    property string sha1Fingerprint : ""

    Column {
        anchors.fill: parent
        anchors.margins: fontSizeSmall
        spacing: paddingTiny

        Label {
            text: qsTr("Would you like to accept the certificate?")
            font.pixelSize: fontSizeMedium
            width: parent.width
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        }
        Label {
            text: qsTr("MD5: %1").arg(md5Fingerprint)
            width: parent.width
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        }
        Label {
            text: qsTr("SHA1: %1").arg(sha1Fingerprint)
            width: parent.width
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        }
    }
}
