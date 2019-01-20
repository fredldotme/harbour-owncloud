import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: buttonRoot
    visible: true
    state: "visible"

    property alias source : avatarImage.source
    property color highlightColor : "#88FFFFFF"
    property color shadowColor : "#88FFFFFF"
    property int radius : 16
    signal clicked()

    states: [
        State { name: "visible" },
        State { name: "invisible" }
    ]
    transitions: [
        Transition {
            from: "visible"
            to: "invisible"
            NumberAnimation {
                target: buttonRoot
                property: "opacity"
                duration: 200
                from: 1.0
                to: 0.0
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            from: "invisible"
            to: "visible"
            NumberAnimation {
                target: buttonRoot
                property: "opacity"
                duration: 200
                from: 0.0
                to: 1.0
                easing.type: Easing.InOutQuad
            }
        }
    ]

    layer.enabled: true
    layer.effect: OpacityMask {
        maskSource: Item {
            width: avatarImage.width
            height: avatarImage.height
            Rectangle {
                id: circleCut
                width: parent.width-(buttonRoot.radius*2)
                height: parent.height-(buttonRoot.radius*2)
                anchors.centerIn: parent
                radius: Math.min(width, height)
                visible: false
            }
            DropShadow {
                id: blurShadow
                source: circleCut
                radius: buttonRoot.radius
                samples: 32
                anchors.fill: circleCut
                color: shadowColor
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
        color: highlightColor
        visible: inputArea.pressed
    }

    MouseArea {
        id: inputArea
        anchors.fill: parent
        onClicked: buttonRoot.clicked()
    }
}
