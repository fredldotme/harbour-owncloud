import QtQuick 2.2

Row {
    spacing: 8
    property alias label : detailLabel.text
    property alias value : detailValue.text

    Text {
        id: detailLabel
        font.bold: true
        width: parent.width / 2
        horizontalAlignment: Text.AlignRight
    }
    Text {
        id: detailValue
        width: parent.width / 2
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }
}
