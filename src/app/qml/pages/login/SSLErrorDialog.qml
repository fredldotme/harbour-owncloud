import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Dialog {

    property NextcloudSettings settings : null

    id: sslDialog
    property string md5Digest;
    property string sha1Digest;

    SilicaListView {
        anchors.fill: parent

        DialogHeader {
            id: header
            acceptText: "Accept"
            cancelText: "Reject"
        }

        Label {
            id: hintLabel
            text: qsTr("Do you want to accept this certificate?")
            anchors.top: header.bottom
            anchors.topMargin: 32
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: md5Hint
            anchors.top: hintLabel.bottom
            anchors.topMargin: 32
            text: qsTr("MD5 fingerprint:")
        }

        Label {
            id: md5Label
            text: md5Digest
            font.pixelSize: Theme.fontSizeSmall
            wrapMode: Text.WrapAnywhere
            width: parent.width
            anchors.top: md5Hint.bottom
        }

        Label {
            id: sha1Hint
            anchors.top: md5Label.bottom
            anchors.topMargin: 20
            text: qsTr("SHA1 fingerprint:")
        }

        Label {
            id: sha1Label
            text: sha1Digest
            font.pixelSize: Theme.fontSizeSmall
            wrapMode: Text.WrapAnywhere
            width: parent.width
            anchors.top: sha1Hint.bottom
        }
    }

    onAccepted: {
        if (settings.null) {
            console.warn("Settings nullpointer ref, not accepting certificate")
            return;
        }

        settings.acceptCertificate(md5Digest, sha1Digest);
        settings.readSettings()
    }
}
