import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0

Image {
    id: avatarImage

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

    MouseArea {
        anchors.fill: parent
        onClicked: avatarImage.clicked()
    }
}
