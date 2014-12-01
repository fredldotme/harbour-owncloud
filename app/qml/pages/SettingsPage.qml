import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: pageRoot
    anchors.fill: parent

    Component.onCompleted: {
        settings.readSettings();
    }

    Connections {
        target: browser
        onLoginFailed: {
            loginInProgress = false;
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            id: pulley
            MenuItem {
                text: "Reset connection settings"
                onClicked: {
                    settings.resetSettings();
                    pageStack.clear();
                    pageStack.push("Login.qml");
                }
            }
        }

        TextSwitch {
            id: autoLoginSwitch
            enabled: !loginInProgress
            anchors.top: pulley.bottom
            anchors.topMargin: 64
            text: "Login automatically"
            checked: settings.autoLogin
        }
    }
}
