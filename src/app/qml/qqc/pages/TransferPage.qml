import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import harbour.owncloud 1.0
import "qrc:/qml-ui-set"

Dialog {
    id: pageRoot
    title: qsTr("Transfers")
    modality: "NonModal"

    property AccountWorkerGenerator accountGenerator : null
    property var activeAccountWorkers : accountGenerator.accountWorkers

    Item {
        anchors.fill: parent
        HidingPanel {
            id: daemonProgress

            readonly property bool __visible : (state === "visible")

            clip: true
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: __visible ?
                        paddingLarge * daemonProgress.opacity :
                        0
            state: (daemonCtrl.daemonInstalled & daemonCtrl.uploading) ?
                       "visible" :
                       "invisible"

            AbortableBusyIndicator {
                id: indicator
                height: parent.height
                width: height
                anchors.left: parent.left
                anchors.leftMargin: fontSizeLarge
                anchors.top: parent.top
                anchors.verticalCenter: parent.verticalCenter
                running: daemonProgress.__visible && daemonCtrl.uploading
                onAbort: daemonCtrl.abort()
            }
            Label {
                id: backupText
                text: qsTr("Camera backup in progress...")
                anchors.left: indicator.right
                anchors.leftMargin: fontSizeLarge
                anchors.right: parent.right
                anchors.rightMargin: fontSizeLarge
                anchors.verticalCenter: indicator.verticalCenter
            }
        }

        Flickable {
            anchors.top: daemonProgress.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            contentHeight: height

            Column {
                id: flickableRoot
                anchors.fill: parent

                Repeater {
                    id: groupDelegate
                    model: activeAccountWorkers

                    Column {
                        width: parent.width

                        property AccountWorkers accountWorker : activeAccountWorkers[index]
                        property CloudStorageProvider transferQueue : accountWorker.transferCommandQueue

                        Label {
                            visible: transferQueue.queue.length < 1
                            text: qsTr("No pending file transfers")
                            font.pixelSize: fontSizeMedium
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Row {
                            spacing: paddingLarge
                            width: parent.width
                            visible: transferQueue.queue.length > 0

                            Repeater {
                                id: accountDelegate
                                width: parent.width
                                height: 64
                                model: transferQueue.queue

                                Column {
                                    id: delegate
                                    width: parent.width
                                    height: 64

                                    property CommandEntity commandEntity : transferQueue.queue[index]

                                    property string commandType :
                                        commandEntity.info.property("type") !== undefined ?
                                            commandEntity.info.property("type") : ""

                                    property string fileName :
                                        commandEntity.info.property("fileName") !== undefined ?
                                            commandEntity.info.property("fileName") : ""

                                    function getIconForCommandType() {
                                        if (commandType == "fileDownload")
                                            return "escape-direction-down"
                                        if (commandType == "fileUpload")
                                            return "escape-direction-up"
                                        if (commandType == "fileOpen")
                                            return "folder-open"
                                        return ""
                                    }

                                    // The transfer queue could contain additional entities,
                                    // including prop patch requests. Hide all except for
                                    // downloads, uploads and file opening requests.
                                    property bool isTransfer :
                                        (commandType === "fileDownload" ||
                                         commandType === "fileUpload")

                                    readonly property bool isFileOpen :
                                        (commandType === "fileOpen")

                                    readonly property string openFileHint :
                                        qsTr("Open: '%1'").arg(fileName)

                                    property bool isAllowedVisible :
                                        (isTransfer || isFileOpen)

                                    Component.onCompleted: {
                                        if (!isAllowedVisible) height = 0
                                    }
                                    visible: isAllowedVisible

                                    Button {
                                        id: transferDescription
                                        width: parent.width
                                        icon.name: delegate.getIconForCommandType()
                                        icon.color: "transparent"
                                        background: Rectangle { color: "transparent" }
                                        text: !delegate.isFileOpen ? delegate.fileName +
                                                                     " (" + accountWorker.account.hoststring + ")"
                                                                   : delegate.openFileHint
                                    }

                                    Row {
                                        width: parent.width

                                        Button {
                                            id: transferCancelButton
                                            icon.name: "view-close"
                                            icon.color: "transparent"
                                            background: Rectangle { color: "transparent" }
                                            onClicked: {
                                                delegate.commandEntity.abort(true)
                                            }
                                        }

                                        ProgressBar {
                                            id: transferProgress
                                            width: parent.width - transferCancelButton.width
                                            visible: delegate.isTransfer
                                            indeterminate: false
                                            from: 0.0
                                            to: 1.0
                                            value: delegate.commandEntity.progress
                                        }
                                    }
                                }

                            }
                        }
                    }
                }
            }
        }
    }
}
