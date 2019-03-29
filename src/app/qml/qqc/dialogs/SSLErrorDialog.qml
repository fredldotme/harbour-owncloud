import QtQuick 2.0
import QtQuick.Controls 2.2

Dialog {
    id: textEntryDialog
    standardButtons: Dialog.Ok | Dialog.Cancel
    height: 100

    property string md5Fingerprint : ""
    property string sha1Fingerprint : ""

    Column {
        anchors.fill: parent
        anchors.margins: fontSizeSmall
        Label {
            text: qsTr("Would you like to accept the certificate?")
            font.pixelSize: fontSizeLarge
        }
        Label {
            text: qsTr("MD5: %1").arg(md5Fingerprint)
        }
        Label {
            text: qsTr("SHA1: %1").arg(sha1Fingerprint)
        }
    }
}
