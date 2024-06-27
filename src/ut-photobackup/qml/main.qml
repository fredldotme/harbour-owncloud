import QtQuick 2.0
import Lomiri.Components 1.3
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import harbour.owncloud 1.0

ApplicationWindow {
    id: rootWindow
    objectName: "rootWindow"
    title: qsTr("GhostCloud Photo Backup")
    visible: true
    width: 600
    minimumWidth: 320
    height: 400
    minimumHeight: 380

    ServiceCreator {
        id: serviceCreator
    }

    DaemonControl {
        id: daemonCtrl
    }

    AccountsDb {
        id: accountsDb
    }

    onActiveChanged: {
        accountsDb.refresh()
    }

    Flickable {
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height

        Column {
            id: mainColumn
            anchors.fill: parent
            spacing: units.gu(3)

            Image {
                x: (parent.width - width) / 2
                width: Math.min(rootWindow.width, rootWindow.height) / 2
                height: width
                source: "qrc:/icons/icon_gray.svg"
                sourceSize.width: width
                sourceSize.height: height
            }

            Label {
                width: parent.width
                font.pixelSize: units.gu(4)
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Label.WordWrap
                text: qsTr("GhostCloud Photo Backup")
            }

            Column {
                width: parent.width
                spacing: units.gu(1.5)
                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Label.WordWrap
                    text: qsTr("Step 1: Select your preferred photo backup storage account")
                }
                Button {
                    x: (parent.width - width) / 2
                    text: qsTr("Select an account")
                    onClicked: {
                        Qt.openUrlExternally("settings:///system/online-accounts")
                    }
                }
            }

            Column {
                width: parent.width
                spacing: units.gu(1)
                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Label.WordWrap
                    text: qsTr("Step 2: Enable the backup service")
                }
                Button {
                    x: (parent.width - width) / 2
                    text: serviceCreator.serviceEnabled ? qsTr("Disable service") : qsTr("Enable service")
                    enabled: accountsDb.accounts.length > 0
                    onClicked: {
                        serviceCreator.serviceEnabled = !serviceCreator.serviceEnabled
                    }
                }
            }

            Column {
                width: parent.width
                spacing: units.gu(1)
                Button {
                    x: (parent.width - width) / 2
                    text: qsTr("Retry sync")
                    enabled: accountsDb.accounts.length > 0
                    onClicked: {
                        daemonCtrl.reloadConfig()
                    }
                }
            }

            Label {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                visible: accountsDb.accounts.length > 0
                text: daemonCtrl.uploading ? qsTr("Uploading photos...") : qsTr("Not uploading right now.")
            }

            BusyIndicator {
                width: units.gu(6)
                height: width
                running: daemonCtrl.uploading
                x: (parent.width - width) / 2
            }
        }
    }
}
