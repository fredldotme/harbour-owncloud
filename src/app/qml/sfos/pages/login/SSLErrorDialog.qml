import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Dialog {
    property AccountBase settings : null

    id: sslDialog
    anchors.fill: parent
    property string md5Digest;
    property string sha1Digest;

    SilicaListView {
        anchors.fill: parent

        DialogHeader {
            id: header
        }

        Label {
            id: hintLabel
            text: qsTr("Do you want to accept this certificate?")
            font.pixelSize: Theme.fontSizeExtraLarge
            color: Theme.highlightColor
            anchors.top: header.bottom
            anchors.topMargin: Theme.paddingLarge
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: Theme.paddingMedium
            anchors.rightMargin: Theme.paddingMedium
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        Label {
            id: md5Hint
            anchors.top: hintLabel.bottom
            anchors.topMargin: Theme.paddingLarge
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingMedium
            text: qsTr("MD5 fingerprint:")
        }

        Label {
            id: md5Label
            text: md5Digest
            font.pixelSize: Theme.fontSizeMedium
            wrapMode: Text.WrapAnywhere
            width: parent.width
            anchors.top: md5Hint.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge
            anchors.rightMargin: Theme.paddingLarge
        }

        Label {
            id: sha1Hint
            anchors.top: md5Label.bottom
            anchors.topMargin: 32
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingMedium
            text: qsTr("SHA1 fingerprint:")
        }

        Label {
            id: sha1Label
            text: sha1Digest
            font.pixelSize: Theme.fontSizeMedium
            wrapMode: Text.WrapAnywhere
            width: parent.width
            anchors.top: sha1Hint.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: Theme.paddingLarge
            anchors.rightMargin: Theme.paddingLarge
        }
    }

    onAccepted: {
        if (settings === null) {
            console.warn("Settings nullpointer ref, not accepting certificate")
            return;
        }

        settings.acceptTlsFingerprints(md5Digest, sha1Digest);
    }
}
