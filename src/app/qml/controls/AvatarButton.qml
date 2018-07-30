import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: buttonRoot

    property alias source : avatarImage.source
    property color highlightColor : "white"
    signal clicked()

    layer.enabled: true
    layer.effect: OpacityMask {
        maskSource: Item {
            width: avatarImage.width
            height: avatarImage.height
            Rectangle {
                anchors.fill: parent
                radius: Math.min(width, height)
            }
        }
    }

    Image {
        id: avatarImage
        anchors.fill: parent
        visible: !inputArea.pressed
    }

    ColorOverlay {
        id: pressedColorOverlay
        anchors.fill: avatarImage
        source: avatarImage
        color: Qt.rgba(highlightColor.r,
                       highlightColor.g,
                       highlightColor.b,
                       0.5)
        visible: inputArea.pressed
    }

    MouseArea {
        id: inputArea
        anchors.fill: parent
        onClicked: buttonRoot.clicked()
    }
}
