import QtQuick 2.0

Item {
    id: controlRoot

    state: visible ? "visible" : "invisible"
    visible: height > 0
    states: [
        State {
            name: "visible"
        },
        State {
            name: "invisible"
        }
    ]
    transitions: [
        Transition {
            from: "invisible"
            to: "visible"
            NumberAnimation {
                target: controlRoot
                property: "opacity"
                duration: 150
                from: 0.0
                to: 1.0
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            from: "visible"
            to: "invisible"
            NumberAnimation {
                target: controlRoot;
                property: "opacity"
                duration: 150
                from: 1.0
                to: 0.0
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
