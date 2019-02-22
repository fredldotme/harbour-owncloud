import QtQuick 2.0
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

BusyIndicator {
    id: controlRoot
    enabled: running
    visible: running

    property int buttonVisibiltyDelay : 0
    signal abort()

    readonly property bool __showButton :
        (controlRoot.visible && !abortButtonHideTimer.running)

    onRunningChanged: {
        if (running)
            abortButtonHideTimer.restart()
    }

    Item {
        id: buttonContainer
        anchors.fill: parent
        state: __showButton ? "visible" : "invisible"
        states: [
            State { name: "visible" },
            State { name: "invisible" }
        ]
        transitions: [
            Transition {
                from: "visible"
                to: "invisible"
                NumberAnimation {
                    target: buttonContainer
                    property: "opacity"
                    duration:   100
                    from: 1.0
                    to: 0.0
                    easing.type: Easing.InOutQuad
                }
            },
            Transition {
                from: "invisible"
                to: "visible"
                NumberAnimation {
                    target: buttonContainer
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
            source: "qrc:/icons/theme/actions/symbolic/window-close-symbolic.svg"
            anchors.fill: parent
            enabled: false
            sourceSize.width: width
            sourceSize.height: height
            visible: (!abortButtonHideTimer.running && !abortButtonMouseArea.pressed)
        }
        ColorOverlay {
            // TODO: decide if adding a Silica-style "Theme.secondaryHighlightColor" constant makes sense
            property color __highlightColor : "orange"
            source: abortButtonImage
            anchors.fill: parent
            visible: (!abortButtonHideTimer.running && abortButtonMouseArea.pressed)
            color: Qt.rgba(__highlightColor.r,
                           __highlightColor.g,
                           __highlightColor.b,
                           0.5)
        }
    }

    Timer {
        id: abortButtonHideTimer
        interval: buttonVisibiltyDelay
    }

    MouseArea {
        id: abortButtonMouseArea
        anchors.fill: parent
        enabled: __showButton
        onClicked: {
            controlRoot.abort()
        }
    }
}
