import QtQuick 2.0
import Sailfish.Silica 1.0

BackgroundItem {
    id: notification
    width: parent.width

    onVisibleChanged: {
        animationIn.start()
    }

    Timer {
        id: timerOut
        interval: 2000
        repeat: false
        running: false
        onTriggered: {
            animationOut.start()
        }
    }

    NumberAnimation {
        id: animationIn
        target: notification;
        property: "opacity";
        duration: 200;
        running: false;
        easing.type: Easing.InOutQuad;
        from: 0.0;
        to: 1.0
        onStopped: {
            timerOut.start()
        }
    }

    NumberAnimation {
        id: animationOut
        target: notification;
        property: "opacity";
        running: false;
        duration: 300;
        easing.type: Easing.OutInQuad;
        from: 1.0;
        to: 0.0
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.highlightBackgroundColor
    }

    Label {
        text: "Login failed"
        color: "black"
        anchors.centerIn: parent
    }
}
