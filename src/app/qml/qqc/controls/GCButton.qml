import QtQuick 2.3
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

AbstractButton {
    id: buttonRoot

    property alias textFont : label.font
    property alias detailText : detailLabel.text
    property alias detailTextFont : detailLabel.font
    property alias source : img.source
    property alias sourceSize : img.sourceSize
    property bool forcePressed : false

    signal clicked()

    Layout.minimumWidth: width
    Layout.minimumHeight: height

    Rectangle {
        color: "lightgray"
        anchors.fill: parent
        visible: mouseArea.pressed || forcePressed
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
        }
        Column {
            height: parent.height
            width: parent.width - img.width
            anchors.verticalCenter: parent.verticalCenter
            Label {
                id: label
                text: buttonRoot.text
                font.pixelSize: fontSizeSmall
                enabled: buttonRoot.enabled
                elide: Label.ElideRight
                width: parent.width
                color: enabled ? "black"
                               : "lightgray"
            }
            Label {
                id: detailLabel
                font.pixelSize: fontSizeTiny
                enabled: buttonRoot.enabled
                elide: Label.ElideRight
                width: parent.width
                color: enabled ? "black"
                               : "lightgray"
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            if (forcePressed || pressed)
                return;
            buttonRoot.clicked()
        }
        enabled: buttonRoot.enabled
    }
}
