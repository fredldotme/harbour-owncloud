import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2


Dialog {
    id: textEntryDialog
    standardButtons: Dialog.Ok | Dialog.Cancel
    height: 100

    property alias text : textEntryField.text

    Column {
        anchors.fill: parent
        anchors.margins: fontSizeSmall
        TextField {
            id: textEntryField
            width: parent.width
            text: ""
            focus: true
        }
    }

    onVisibleChanged: {
        if (visible)
            textEntryField.focus = true
    }
}
