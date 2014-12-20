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

        PageHeader {
            title: "Settings"
        }

        PullDownMenu {
            id: pulley
            MenuItem {
                text: "Reset connection settings"
                onClicked: {
                    browser.resetWebdav();
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
            anchors.topMargin: 96
            text: "Login automatically"
            description: "Automatically log in to your ownCloud server when starting the app"
            checked: settings.autoLogin
            onCheckedChanged: {
                settings.autoLogin = checked
                settings.writeSettings()
            }
        }
    }
}
