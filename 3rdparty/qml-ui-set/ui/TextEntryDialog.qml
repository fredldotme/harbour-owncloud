import QtQuick 2.6
import QtQuick.Controls 2.4

Dialog {
    id: textEntryDialog
    standardButtons: Dialog.Ok | Dialog.Cancel
    height: 100

    property alias text : textEntryField.text

    TextField {
        anchors.fill: parent
        anchors.margins: 16
        id: textEntryField
        width: parent.width
        text: ""
        focus: true
    }

    onVisibleChanged: {
        if (visible)
            textEntryField.focus = true
    }
}
