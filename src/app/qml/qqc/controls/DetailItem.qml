import QtQuick 2.2

Item {
    property alias label : detailLabel.text
    property alias value : detailValue.text

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 8

        Text {
            id: detailLabel
            font.bold: true
        }
        Text {
            id: detailValue
        }
    }
}
