import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import SailfishUiSet 1.0
import Nemo.Notifications 1.0

Page {
    id: pageRoot
    anchors.fill: parent

    property string remotePath : "/"
    property string pageHeaderText : "/"

    // Keep track of directory listing requests
    property var listCommand : null

    function refreshListView(refresh) {
        listCommand = browserCommandQueue.directoryListingRequest(remotePath, refresh)
    }

    function preventResourceModification(target) {
        var queueInfos = transferQueue.queueInformation();
        for (var i = 0; i < queueInfos.length; i++) {
            var pendingTransfer = queueInfos[i];

            var isTransfer = (pendingTransfer.property("type") === "fileDownload" ||
                              pendingTransfer.property("type") === "fileUpload")

            if (!isTransfer)
                continue;

            if (pendingTransfer.property("remotePath").indexOf(target.path) === 0) {
                return true
            }
        }
        return false
    }

    FileDetailsHelper { id: fileDetailsHelper }
    Notification {
        id: transientNotifier
        isTransient: true
    }

    onStatusChanged: {
        if (status === PageStatus.Inactive) {
            if (_navigation !== undefined && _navigation === PageNavigation.Back) {
                if (listCommand !== null)
                    listCommand.abort()
                directoryContents.remove(remotePath)
                pageRoot.destroy()
            }
        }
    }

    onRemotePathChanged: {
        if (remotePath === "/") {
            pageHeaderText = "/";
        } else {
            var dirs = remotePath.split("/")
            if(dirs.length > 1) {
                pageHeaderText = dirs[dirs.length - 2]
            } else {
                pageHeaderText = ""
            }
        }
    }

    Connections {
        target: browserCommandQueue
        onCommandFinished: {
            // Invalidate listCommand after completion
            if (receipt.info.property("type") === "davList") {
                listCommand = null
                return;
            }

            if (receipt.info.property("remotePath") === pageRoot.remotePath) {
                refreshListView(true)
                return;
            }
        }
    }

    Connections {
        target: transferQueue
        onCommandFinished: {
            if (receipt.info.property("remotePath") === pageRoot.remotePath) {
                refreshListView(true)
                return;
            }
        }
    }

    Connections {
        target: directoryContents
        onInserted: {
            // console.log("key:" + key + " remotePath:" + remotePath)
            if (key !== remotePath)
                return;

            // TODO: apply delta between directoryContents and localListModel
            // TODO: animation when inserting/deleting entries
            listView.model = directoryContents.value(key)
        }
    }

    property var selectedEntry : null;
    property BackgroundItem selectedItem : null;
    property var dialogObj : null

    function renameEntry(tmpEntry, newName) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = remotePath + newName +
                (tmpEntry.isDirectory ? "/" : "")

        browserCommandQueue.moveRequest(fromPath, toPath, true)
        refreshListView(true)
    }

    function moveEntry(tmpEntry, remotePath) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = dialogObj.remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")

        browserCommandQueue.moveRequest(fromPath, toPath, true)
        refreshListView(true)
    }

    function copyEntry(tmpEntry, remotePath) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = dialogObj.remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")

        browserCommandQueue.copyRequest(fromPath, toPath, true)
    }

    function deleteEntry(fullPath) {
        browserCommandQueue.removeRequest(fullPath);
        refreshListView(true)
    }

    SilicaFlickable {
        anchors.fill: parent

        SilicaListView {
            id: listView
            anchors.fill: parent
            clip: true
            model: directoryContents.value(remotePath)

            add: Transition {
                NumberAnimation {
                    properties: "height"
                    from: 0
                    to: delegate.height
                    duration: 200
                }
            }
            remove: Transition {
                NumberAnimation {
                    properties: "height"
                    from: delegate.height
                    to: 0
                    duration: 200
                }
            }

            header: PageHeader {
                id: pageHeader
                title: pageHeaderText
            }

            /*BusyIndicator {
                anchors.right: listView.header.left
                anchors.top: listView.header.top
                anchors.bottom: listView.header.bottom
                width: height
                running: browserCommandQueue.running
            }*/

            PullDownMenu {
                MenuItem {
                    text: qsTr("Refresh")
                    enabled: listView.model !== undefined
                    onClicked: {
                        refreshListView(true)
                    }
                }

                MenuItem {
                    text:qsTr("Create directory")
                    enabled: listView.model !== undefined
                    onClicked: {
                        dialogObj = textEntryDialogComponent.createObject(pageRoot);
                        dialogObj.placeholderText = qsTr("Directory name")
                        dialogObj.labelText = dialogObj.placeholderText
                        dialogObj.accepted.connect(function () {
                            var newPath = remotePath + dialogObj.text
                            browserCommandQueue.makeDirectoryRequest(newPath)
                            refreshListView(true)
                            dialogObj = null
                        })
                        pageStack.push(dialogObj)
                    }
                }

                MenuItem {
                    function enqueueSelectedFiles() {
                        var selectedFiles = dialogObj.filesToSelect
                        for (var i = 0; i < selectedFiles.length; i++) {
                            var canonicalRemotePath = FilePathUtil.getCanonicalPath(remotePath);
                            transferQueue.fileUploadRequest(selectedFiles[i].path,
                                                            canonicalRemotePath,
                                                            selectedFiles[i].lastModified);
                        }
                        transferQueue.run()
                        dialogObj = null
                    }

                    text: qsTr("Upload")
                    enabled: listView.model !== undefined
                    onClicked: {
                        dialogObj = selectionDialogComponent.createObject(pageRoot,
                                                                          {maximumSelections:Number.MAX_VALUE});
                        dialogObj.acceptText = qsTr("Upload")
                        dialogObj.errorOccured.connect(function(errorStr) {
                            transientNotifier.previewSummary = errorStr
                            transientNotifier.publish()
                        });
                        dialogObj.accepted.connect(enqueueSelectedFiles)
                        dialogObj.rejected.connect(function() {
                            dialogObj = null
                        });
                        pageStack.push(dialogObj)
                    }
                }
            }
            PushUpMenu {
                MenuItem {
                    text: qsTr("File transfers")
                    onClicked: {
                        pageStack.push(transferPageComponent)
                    }
                }

                MenuItem {
                    text: qsTr("Settings")
                    onClicked: {
                        pageStack.push(settingsPageComponent)
                    }
                }
            }

            delegate: ListItem {
                id: delegate

                property var davInfo : listView.model[index]

                Image {
                    id: icon
                    source: davInfo.isDirectory ?
                                "image://theme/icon-m-folder" :
                                fileDetailsHelper.getIconFromMime(davInfo.mimeType)
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.top: parent.top
                    anchors.topMargin: 18
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 18
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: label
                    x: icon.x + icon.width + 12
                    y: icon.y - icon.height + 6
                    text: davInfo.name
                    anchors.verticalCenter: parent.verticalCenter
                    color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                }

                onClicked: {
                    if(davInfo.isDirectory) {
                        var nextPath = remotePath + davInfo.name + "/";
                        listCommand = browserCommandQueue.directoryListingRequest(nextPath, false)
                    } else {
                        var fileDetails = fileDetailsComponent.createObject(pageRoot, { entry: davInfo });
                        pageStack.push(fileDetails);
                    }
                }
                onPressAndHold: {
                    selectedEntry = davInfo;
                    selectedItem = delegate
                    if (!menu)
                        menu = contextMenuComponent.createObject(listView, {
                                                                     tmpEntry : selectedEntry
                                                                 })
                    menu.closed.connect(function(){
                        menu.destroy()
                        menu = null
                    });
                    openMenu()
                }
            }
            VerticalScrollDecorator {}

            BusyIndicator {
                anchors.centerIn: parent
                running: listCommand !== null
                size: BusyIndicatorSize.Large
            }

            Component {
                id: deleteRemorseComponent

                RemorseItem {
                    id: remorseItem
                    property string pathToResource : ""

                    onCanceled: {
                        refreshListView(true)
                        remorseItem.destroy()
                    }
                    onTriggered: {
                        deleteEntry(pathToResource)
                        remorseItem.destroy()
                    }
                }
            }

            Component {
                id: contextMenuComponent

                ContextMenu {
                    property var tmpEntry : null;

                    onClosed: {
                        selectedEntry = null
                        selectedItem = null
                    }

                    Connections {
                        target: transferQueue
                        onCommandFinished: {
                            renameMenuItem.enabled = !preventResourceModification(tmpEntry)
                            moveMenuItem.enabled = !preventResourceModification(tmpEntry)
                            deleteMenuItem.enabled = !preventResourceModification(tmpEntry)
                        }
                    }

                    MenuItem {
                        id: renameMenuItem
                        text: qsTr("Rename")
                        enabled : !preventResourceModification(tmpEntry)
                        onClicked: {
                            tmpEntry = selectedEntry
                            dialogObj = textEntryDialogComponent.createObject(pageRoot);
                            dialogObj.text = tmpEntry.name
                            dialogObj.acceptText = qsTr("Rename")
                            dialogObj.placeholderText = qsTr("New name")
                            dialogObj.labelText = dialogObj.placeholderText
                            dialogObj.accepted.connect(function () {
                                renameEntry(tmpEntry, dialogObj.text)
                                dialogObj = null
                                tmpEntry = null
                            });
                            dialogObj.rejected.connect(function() {
                                dialogObj = null
                                tmpEntry = null
                            });
                            pageStack.push(dialogObj);
                        }
                    }
                    MenuItem {
                        id: moveMenuItem
                        text: qsTr("Move")
                        enabled : !preventResourceModification(tmpEntry)
                        onClicked: {
                            tmpEntry = selectedEntry
                            dialogObj = remoteDirDialogComponent.createObject(pageRoot, {entry: tmpEntry});
                            dialogObj.accepted.connect(function () {
                                moveEntry(tmpEntry, remotePath)
                                dialogObj = null
                                tmpEntry = null
                            });
                            dialogObj.rejected.connect(function() {
                                dialogObj = null
                                tmpEntry = null
                            });
                            pageStack.push(dialogObj);
                        }
                    }
                    MenuItem {
                        text: qsTr("Copy")
                        onClicked: {
                            tmpEntry = selectedEntry
                            dialogObj = remoteDirDialogComponent.createObject(pageRoot, {entry: tmpEntry});
                            dialogObj.accepted.connect(function () {
                                copyEntry(tmpEntry, remotePath)
                                dialogObj = null
                                tmpEntry = null
                            });
                            dialogObj.rejected.connect(function() {
                                dialogObj = null
                                tmpEntry = null
                            });
                            pageStack.push(dialogObj);
                        }
                    }
                    MenuItem {
                        id: deleteMenuItem
                        text: qsTr("Delete")
                        enabled : !preventResourceModification(tmpEntry)
                        onClicked: {
                            var fullPath = remotePath + selectedEntry.name
                            var remorseItem =
                                    deleteRemorseComponent.createObject(selectedItem,
                                                                        {
                                                                            pathToResource : fullPath
                                                                        });
                            remorseItem.execute(selectedItem,
                                                qsTr("Deleting", "RemorseItem text"))
                        }
                    }
                }
            }
        }
    }
}





