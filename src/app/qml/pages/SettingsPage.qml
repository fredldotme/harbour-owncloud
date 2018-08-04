import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: pageRoot

    Component.onCompleted: {
        persistentSettings.readSettings();
    }

    onStatusChanged: {
        if (status === PageStatus.Deactivating) {
            if (_navigation === PageNavigation.Back) {
                persistentSettings.writeSettings()
                daemonCtrl.reloadConfig()
            }
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        PageHeader {
            id: pageHeader
            title: qsTr("Settings")
        }

        PullDownMenu {
            id: pulley
            MenuItem {
                text: qsTr("Reset connection settings")
                onClicked: {
                    transferQueue.stop()
                    persistentSettings.resetSettings();
                    pageStack.clear();
                    pageStack.push(basicAuthenticationPage);
                }
            }
        }

        Column {
            spacing: Theme.paddingSmall
            anchors {
                top: pageHeader.bottom
                left: pageHeader.left
                right: pageHeader.right
                bottom: pageHeader.bottom
            }

            TextSwitch {
                id: autoLoginSwitch
                text: qsTr("Login automatically")
                description: qsTr("Automatically log in to your ownCloud server when starting the app", "Login automatically description")
                checked: persistentSettings.autoLogin
                onClicked: persistentSettings.autoLogin = checked
            }

            TextSwitch {
                id: notificationSwitch
                text: qsTr("Notifications")
                description: qsTr("Show global notifications when transfering files", "Notifications description")
                checked: persistentSettings.notifications
                onClicked: persistentSettings.notifications = checked
            }

            TextSwitch {
                id: cameraUploadSwitch
                text: qsTr("Camera photo backups")
                description: qsTr("Automatically save camera photos to your ownCloud instance when on WiFi", "Camera photo backups escription")
                visible: daemonCtrl.daemonInstalled
                checked: persistentSettings.uploadAutomatically
                onClicked: persistentSettings.uploadAutomatically = checked
            }

            TextSwitch {
                id: mobileCameraUploadSwitch
                text: qsTr("Photo backups via mobile internet connection")
                description: qsTr("Also automatically backup camera photos when connected via 2G, 3G or LTE", "hoto backups via mobile internet connection description")
                visible: daemonCtrl.daemonInstalled
                enabled: cameraUploadSwitch.checked
                checked: persistentSettings.mobileUpload
                onClicked: persistentSettings.mobileUpload = checked
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Clear cache")
                onClicked: thumbnailCache.clearCache()
            }
        }
    }
}
