import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {

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
            text: "Do you want to accept this certificate?"
            anchors.top: header.bottom
            anchors.topMargin: 32
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: md5Hint
            anchors.top: hintLabel.bottom
            anchors.topMargin: 32
            text: "MD5 fingerprint:"
        }

        Label {
            id: md5Label
            text: md5Digest
            anchors.top: md5Hint.bottom
        }

        Label {
            id: sha1Hint
            anchors.top: md5Label.bottom
            anchors.topMargin: 20
            text: "SHA1 fingerprint:"
        }

        Label {
            id: sha1Label
            text: sha1Digest
            anchors.top: sha1Hint.bottom
        }
    }

    onAccepted: {
        settings.acceptCertificate(md5Digest, sha1Digest);
    }
}
