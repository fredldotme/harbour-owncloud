import QtQuick 2.0
import Sailfish.Silica 1.0

BackgroundItem {
    id: notificationItem
    width: parent.width
    property string notification : "";

    onVisibleChanged: {
        if(visible) {
            animationOut.stop()
            animationIn.start()
        } else {
            animationIn.stop()
            animationOut.stop()
        }
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
        target: notificationItem;
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
        target: notificationItem;
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
        text: notification
        color: "black"
        anchors.centerIn: parent
    }
}
