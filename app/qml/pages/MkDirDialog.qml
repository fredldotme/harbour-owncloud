import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Dialog {
    canAccept: dirName.text !== "";

    property string fileToUpload;
    property string remotePath;

    onAccepted: {
        browser.makeDirectory(dirName.text)
    }

    SilicaFlickable {
        anchors.fill: parent

        DialogHeader {
            id: header
            acceptText: "Accept"
            cancelText: "Cancel"
        }

        TextField {
            id: dirName
            anchors.centerIn: parent
            width: parent.width - 64
            focus: true
            placeholderText: "Directory name"
            label: placeholderText
        }

    }
}
