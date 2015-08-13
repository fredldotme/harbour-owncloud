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

    onStatusChanged: {
        if (status === PageStatus.Deactivating) {
            if (_navigation === PageNavigation.Back) {
                settings.writeSettings()
                daemonCtrl.reloadConfig()
            }
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
            onClicked: settings.autoLogin = checked
        }

        TextSwitch {
            id: notificationSwitch
            anchors.top: autoLoginSwitch.bottom
            text: "Notifications"
            description: "Show global notifications when transfering files"
            checked: settings.notifications
            onClicked: settings.notifications = checked
        }

        TextSwitch {
            id: cameraUploadSwitch
            anchors.top: notificationSwitch.bottom
            text: "Camera photo backups"
            description: "Automatically save camera photos to your ownCloud instance when on WiFi"
            visible: daemonCtrl.daemonInstalled
            checked: settings.uploadAutomatically
            onClicked: settings.uploadAutomatically = checked
        }

        TextSwitch {
            id: mobileCameraUploadSwitch
            anchors.top: cameraUploadSwitch.bottom
            text: "Photo backups via mobile internet connection"
            description: "Also automatically backup camera photos when connected via 2G, 3G or LTE"
            visible: daemonCtrl.daemonInstalled
            enabled: cameraUploadSwitch.checked
            checked: settings.mobileUpload
            onClicked: settings.mobileUpload = checked
        }
    }
}
