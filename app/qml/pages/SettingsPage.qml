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

        TextSwitch {
            id: notificationSwitch
            anchors.top: autoLoginSwitch.bottom
            text: "Notifications"
            description: "Show global notifications when transfering files"
            checked: settings.notifications
            onCheckedChanged: {
                settings.notifications = checked
                settings.writeSettings()
            }
        }

        TextSwitch {
            id: cameraUploadSwitch
            anchors.top: autoLoginSwitch.bottom
            text: "Camera photo backups"
            description: "Automatically save camera photos to your ownCloud instance when on WiFi"
            visible: daemonCtrl.daemonInstalled
            checked: settings.uploadAutomatically
            onCheckedChanged: {
                settings.uploadAutomatically = checked
                settings.writeSettings()
            }
        }
    }
}
