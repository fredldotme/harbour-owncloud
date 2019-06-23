import QtQuick 2.0
import QtQuick.Controls 2.2
import harbour.owncloud 1.0
import "qrc:/qml/qqc/controls"
import "qrc:/qml-ui-set"

Dialog {
    id: pageRoot

    signal closeRequest()

    readonly property bool destroyable : false

    property AccountWorkerGenerator accountGenerator : null
    property var activeAccountWorkers : accountGenerator.accountWorkers
    property bool noTransfersHint : true

    standardButtons: Dialog.Ok

    function refreshNoTransfersHint()
    {
        noTransfersHint = true
        for (var i = 0; i < activeAccountWorkers.length; i++) {
            var transferCommandQueue = activeAccountWorkers[i].transferCommandQueue
            var queueLength = transferCommandQueue.queue.length;
            if (queueLength < 1)
                continue

            noTransfersHint = false
            break
        }
    }

    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: refreshNoTransfersHint()
    }

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
        id: flickableRoot
        anchors.top: daemonProgress.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        contentHeight: groupDelegate.height
        clip: true

        Column {
            id: groupDelegate
            anchors.fill: parent

            Repeater {
                model: activeAccountWorkers

                delegate: Row {
                    property AccountWorkers accountWorker : activeAccountWorkers[index]
                    property CloudStorageProvider transferQueue : accountWorker.transferCommandQueue

                    spacing: paddingLarge
                    width: flickableRoot.width
                    visible: transferQueue.queue.length > 0

                    Repeater {
                        id: accountDelegate
                        width: parent.width
                        height: 64
                        model: transferQueue.queue

                        delegate: Column {
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
                                    return "qrc:/icons/theme/actions/32/go-down.svg"
                                if (commandType == "fileUpload")
                                    return "qrc:/icons/theme/actions/32/go-up.svg"
                                if (commandType == "fileOpen")
                                    return "qrc:/icons/theme/places/64/folder.svg"
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
                                //if (!isAllowedVisible) height = 0
                                refreshNoTransfersHint()
                            }
                            visible: isAllowedVisible

                            GCButton {
                                id: transferDescription
                                width: parent.width
                                height: 48
                                source: delegate.getIconForCommandType()
                                enabled: false
                                background: Rectangle { color: "transparent" }
                                text: !delegate.isFileOpen ? delegate.fileName
                                                           : delegate.openFileHint
                                detailText: delegate.isFileOpen
                                            ? accountWorker.account.hoststring
                                            : ""
                            }

                            Row {
                                width: parent.width

                                GCButton {
                                    id: transferCancelButton
                                    source: "qrc:/icons/theme/actions/symbolic/window-close-symbolic.svg"
                                    height: 32
                                    width: 32
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

    Label {
        visible: noTransfersHint
        text: qsTr("No pending file transfers")
        font.pixelSize: fontSizeLarge
        width: parent.width
        height: parent.height
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }
}
