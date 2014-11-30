import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {

    id: sslDialog
    property string md5Digest;
    property string sha1Digest;

    DialogHeader {
        id: header
        acceptText: "Accept"
        cancelText: "Reject"
    }

    SilicaListView {
        anchors.fill: parent
        Label {
            id: hintLabel
            text: "Do you want to accept this certificate?"
            anchors.top: header.bottom
            anchors.topMargin: 32
        }

        Label {
            id: md5Label
            text: md5Digest
            anchors.top: hintLabel.bottom
        }

        Label {
            id: sha1Label
            text: sha1Digest
            anchors.top: md5Label.bottom
        }
    }

    onAccepted: {
        console.log("DERE")
        settings.acceptCertificate(md5Digest, sha1Digest);
        browser.getDirectoryContent("/");
    }
}
