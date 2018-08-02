import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0

BusyIndicator {
    id: controlRoot
    enabled: running
    visible: running
    size: BusyIndicatorSize.Large

    property int buttonVisibiltyDelay : 0
    signal abort()

    onRunningChanged: {
        if (running)
            abortButtonHideTimer.restart()
    }

    Timer {
        id: abortButtonHideTimer
        interval: buttonVisibiltyDelay
    }
    Item {
        id: abortButtonContainer
        anchors.fill: parent
        visible: !abortButtonHideTimer.running
        state: visible ? "visible" : "invisible"
        states: [
            State { name: "visible" },
            State { name: "invisible" }
        ]
        transitions: [
            Transition {
                from: "visible"
                to: "invisible"
                NumberAnimation {
                    target: abortButtonContainer
                    property: "opacity"
                    duration: 100
                    from: 1.0
                    to: 0.0
                    easing.type: Easing.InOutQuad
                }
            },
            Transition {
                from: "invisible"
                to: "visible"
                NumberAnimation {
                    target: abortButtonContainer
                    property: "opacity"
                    duration: 100
                    from: 0.0
                    to: 1.0
                    easing.type: Easing.InOutQuad
                }
            }
        ]
        Image {
            id: abortButtonImage
            source: "image://theme/icon-l-cancel"
            anchors.fill: parent
            visible: !abortButtonMouseArea.pressed
        }
        ColorOverlay {
            property color __highlightColor : Theme.secondaryHighlightColor
            source: abortButtonImage
            anchors.fill: parent
            visible: abortButtonMouseArea.pressed
            color: Qt.rgba(__highlightColor.r,
                           __highlightColor.g,
                           __highlightColor.b,
                           0.5)
        }
    }

    MouseArea {
        id: abortButtonMouseArea
        anchors.fill: parent
        enabled: abortButtonContainer.visible
        onClicked: {
            controlRoot.abort()
        }
    }
}
