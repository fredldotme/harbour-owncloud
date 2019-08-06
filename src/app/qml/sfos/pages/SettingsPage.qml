import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Page {
    id: pageRoot

    property var accountWorkers : null
    property AccountDb accountDb : null

    onStatusChanged: {
        if (status === PageStatus.Deactivating) {
            if (_navigation === PageNavigation.Back) {
                accountDb.updateAccount(accountWorkers.account)
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

        Column {
            spacing: Theme.paddingSmall
            anchors {
                top: pageHeader.bottom
                left: pageHeader.left
                right: pageHeader.right
                bottom: pageHeader.bottom
            }

            TextSwitch {
                id: cameraUploadSwitch
                text: qsTr("Camera photo backups")
                description: qsTr("Automatically save camera photos to your ownCloud instance when on WiFi", "Camera photo backups escription")
                visible: daemonCtrl.daemonInstalled
                checked: accountWorkers.account.uploadAutomatically
                onClicked: accountWorkers.account.uploadAutomatically = checked
            }

            TextSwitch {
                id: mobileCameraUploadSwitch
                text: qsTr("Photo backups via mobile internet connection")
                description: qsTr("Also automatically backup camera photos when connected via 2G, 3G or LTE", "hoto backups via mobile internet connection description")
                visible: daemonCtrl.daemonInstalled
                enabled: cameraUploadSwitch.checked
                checked: accountWorkers.account.mobileUpload
                onClicked: accountWorkers.account.mobileUpload = checked
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingLarge
                Button {
                    text: qsTr("Clear cache")
                    onClicked: accountWorkers.cacheProvider.clearCache()
                }

                Button {
                    text: qsTr("Clear downloads")
                    onClicked: accountWorkers.cacheProvider.clearDownloads()
                }
            }
        }
    }
}
