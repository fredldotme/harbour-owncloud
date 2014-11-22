import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {

    id: sslDialog
    property string md5Digest;
    property string sha1Digest;

    DialogHeader {
        acceptText: "Accept"
        cancelText: "Reject"
    }

    SilicaListView {
        Label {
            id: hintLabel
            text: "Do you want to accept this certificate?"
        }

        Label {
            id: md5Label
            text: md5Digest
            anchors.top: hintLabel
        }

        Label {
            id: sha1Label
            text: sha1Digest
            anchors.top: md5Label
        }
    }

    onAccepted: {
        console.log("DERE")
        settings.acceptCertificate(md5Digest, sha1Digest);
    }
}
