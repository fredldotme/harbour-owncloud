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
            title: qsTr("Settings")
        }

        PullDownMenu {
            id: pulley
            MenuItem {
                text: qsTr("Reset connection settings")
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
            text: qsTr("Login automatically")
            description: qsTr("Automatically log in to your ownCloud server when starting the app", "Login automatically description")
            checked: settings.autoLogin
            onClicked: settings.autoLogin = checked
        }

        TextSwitch {
            id: notificationSwitch
            anchors.top: autoLoginSwitch.bottom
            text: qsTr("Notifications")
            description: qsTr("Show global notifications when transfering files", "Notifications description")
            checked: settings.notifications
            onClicked: settings.notifications = checked
        }

        TextSwitch {
            id: cameraUploadSwitch
            anchors.top: notificationSwitch.bottom
            text: qsTr("Camera photo backups")
            description: qsTr("Automatically save camera photos to your ownCloud instance when on WiFi", "Camera photo backups escription")
            visible: daemonCtrl.daemonInstalled
            checked: settings.uploadAutomatically
            onClicked: settings.uploadAutomatically = checked
        }

        TextSwitch {
            id: mobileCameraUploadSwitch
            anchors.top: cameraUploadSwitch.bottom
            text: qsTr("Photo backups via mobile internet connection")
            description: qsTr("Also automatically backup camera photos when connected via 2G, 3G or LTE", "hoto backups via mobile internet connection description")
            visible: daemonCtrl.daemonInstalled
            enabled: cameraUploadSwitch.checked
            checked: settings.mobileUpload
            onClicked: settings.mobileUpload = checked
        }
    }
}
