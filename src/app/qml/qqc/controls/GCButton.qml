import QtQuick 2.3
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

AbstractButton {
    id: buttonRoot

    property alias detailText : detailLabel.text
    property alias source : img.source
    property alias sourceSize : img.sourceSize
    signal clicked()

    Layout.minimumWidth: width
    Layout.minimumHeight: height

    Rectangle {
        color: "lightgray"
        anchors.fill: parent
        visible: mouseArea.pressed
        enabled: buttonRoot.enabled
    }

    Row {
        anchors.fill: parent
        anchors.margins: paddingTiny
        spacing: paddingTiny

        Image {
            id: img
            height: parent.height
            width: height
            //sourceSize.width: width - paddingTiny*2
            //sourceSize.height: height - paddingTiny*2
        }
        Column {
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            Label {
                id: label
                text: buttonRoot.text
                font.pixelSize: fontSizeSmall
                enabled: buttonRoot.enabled
            }
            Label {
                id: detailLabel
                font.pixelSize: fontSizeTiny
                enabled: buttonRoot.enabled
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: buttonRoot.clicked()
        enabled: buttonRoot.enabled
    }
}
