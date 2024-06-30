import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Lomiri.Components 1.3
import harbour.owncloud 1.0

ApplicationWindow {
    id: rootWindow
    objectName: "rootWindow"
    title: "GhostCloud Backup"
    visible: true
    width: 600
    height: 400

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

    MainView {
        anchors.fill: parent

        Page {
            id: mainPage
            header: PageHeader {
                title: "GhostCloud Backup"
                flickable: mainFlickable
                trailingActionBar {
                    actions: [
                        Action {
                            iconName: "sync"
                            text: qsTr("Sync")
                            enabled: accountsDb.accounts.length > 0 && serviceCreator.serviceEnabled
                            onTriggered: {
                                // Might serviceCreator might need a restart for a sync
                                serviceCreator.serviceEnabled = false
                                serviceCreator.serviceEnabled = true
                            }
                        }
                    ]
                }

                ProgressBar {
                    height: units.gu(1)
                    width: parent.width
                    anchors.top: mainPage.header.bottom
                    indeterminate: true
                    visible: daemonCtrl.uploading
                }
            }

            LomiriShape {
                id: statusArea
                width: source.width
                height: source.height
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: mainPage.header.bottom

                source: Item {
                    width: (mainFlickable.width / 3) * 2
                    height: notificationArea.height

                    Rectangle {
                        id: notificationArea
                        width: parent.width
                        height: implicitHeight
                        color: LomiriColors.graphite
                        property string message : daemonCtrl.uploading ? qsTr("Synchronizing...") :
                                                                         qsTr("Not uploading right now.")

                        Label {
                            anchors.fill: parent
                            text: notificationArea.message
                            font.pixelSize: units.gu(2)
                        }
                    }
                }
            }

            Flickable {
                id: mainFlickable
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: mainPage.header.bottom
                anchors.bottom: statusArea.bottom

                Column {
                    anchors.fill: parent
                    anchors.leftMargin: units.gu(1)
                    anchors.rightMargin: units.gu(1)

                    Column {
                        id: mainColumn
                        width: parent.width
                        height: implicitHeight
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
                            text: qsTr("GhostCloud Backup")
                        }

                        Column {
                            width: parent.width
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: units.gu(1.5)

                            Label {
                                width: parent.width
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Label.WordWrap
                                text: qsTr("Step 1: Select your preferred backup storage account")
                            }

                            RowLayout {
                                id: selectAccountRow
                                width: parent.width
                                height: implicitHeight
                                property bool ok: (accountsDb.accounts.length > 0)

                                Item {
                                    Layout.fillWidth: true
                                }
                                Button {
                                    Layout.preferredWidth: implicitWidth
                                    text: qsTr("Select an account")
                                    onClicked: {
                                        Qt.openUrlExternally("settings:///system/online-accounts")
                                    }
                                }
                                Icon {
                                    Layout.preferredWidth: units.gu(2)
                                    Layout.preferredHeight: width
                                    name: selectAccountRow.ok ? "ok" : "stop"
                                    color: selectAccountRow.ok ? LomiriColors.green : LomiriColors.graphite
                                }
                                Item {
                                    Layout.fillWidth: true
                                }
                            }
                        }

                        Column {
                            width: parent.width
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: units.gu(1.5)

                            Label {
                                width: parent.width
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Label.WordWrap
                                text: qsTr("Step 2: Enable the backup service")
                            }

                            RowLayout {
                                id: enableServiceRow
                                width: parent.width
                                height: implicitHeight
                                property bool ok: serviceCreator.serviceEnabled

                                Item {
                                    Layout.fillWidth: true
                                }
                                Button {
                                    Layout.preferredWidth: implicitWidth
                                    text: !serviceCreator.serviceEnabled ? qsTr("Activate service") : qsTr("Deactivate service")
                                    enabled: accountsDb.accounts.length > 0
                                    onClicked: {
                                        serviceCreator.serviceEnabled = !serviceCreator.serviceEnabled
                                    }
                                }
                                Icon {
                                    Layout.preferredWidth: units.gu(2)
                                    Layout.preferredHeight: width
                                    name: selectAccountRow.ok && enableServiceRow.ok ? "ok" : "stop"
                                    color: selectAccountRow.ok && enableServiceRow.ok ? LomiriColors.green : LomiriColors.graphite
                                }
                                Item {
                                    Layout.fillWidth: true
                                }
                            }
                        }

                        Icon {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: units.gu(4)
                            height: width
                            property bool visibility: selectAccountRow.ok && enableServiceRow.ok
                            opacity: visibility ? 1.0 : 0.0
                            visible: opacity > 0.0
                            Behavior on opacity {
                                LomiriNumberAnimation {}
                            }

                            name: daemonCtrl.uploading ? "sync" : "tick"
                            color: LomiriColors.green
                        }
                    }
                }
            }
        }
    }
}
