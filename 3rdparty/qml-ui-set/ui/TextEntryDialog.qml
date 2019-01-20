import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Dialog {
    id: dialogRoot
    canAccept: textEntryField.text !== "";

    property alias text : textEntryField.text
    property string acceptText : header.defaultAcceptText
    property string cancelText : header.defaultCancelText
    property string placeholderText : ""
    property string labelText : ""

    onDone: {
        textEntryField.focus = false
    }

    SilicaFlickable {
        anchors.fill: parent

        DialogHeader {
            id: header
            acceptText: dialogRoot.acceptText
            cancelText: dialogRoot.cancelText
        }

        TextField {
            id: textEntryField
            anchors.centerIn: parent
            width: parent.width - 64
            focus: true
            placeholderText: dialogRoot.placeholderText
            label: dialogRoot.labelText
        }
    }
}
