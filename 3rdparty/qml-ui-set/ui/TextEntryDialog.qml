import QtQuick 2.6
import QtQuick.Controls 2.2

Dialog {
    id: textEntryDialog
    standardButtons: Dialog.Ok | Dialog.Cancel

    property alias text : textEntryField.text

    TextField {
        anchors.fill: parent
        anchors.margins: 16
        id: textEntryField
        width: parent.width
        text: ""
        Keys.onReturnPressed: {
            accept()
        }
    }

    onVisibleChanged: {
        console.log("TextEntryDialog visible " + visible)
        if (visible)
            textEntryField.forceActiveFocus()
    }
}
