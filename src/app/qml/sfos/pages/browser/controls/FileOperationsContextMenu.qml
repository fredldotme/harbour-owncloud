import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

ContextMenu {
    id: contextMenu

    property var selectedEntry : null;
    property var selectedItem : null
    property var dialogObj : null;
    property CloudStorageProvider transferQueue : null
    property CloudStorageProvider browserCommandQueue : null
    property Component remoteDirDialogComponent : null
    property Component textEntryDialogComponent: null
    property Component fileDetailsComponent : null

    signal requestListReload(bool refresh)
    signal contextMenuDone()

    property bool __enableDestructiveMenus :
        !preventResourceModification(selectedEntry)

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

    function renameEntry(tmpEntry, newName) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = remotePath + newName +
                (tmpEntry.isDirectory ? "/" : "")

        browserCommandQueue.moveRequest(fromPath, toPath)
        requestListReload(true)
        contextMenuDone()
    }

    function moveEntry(tmpEntry, newDir) {
        var fromPath = tmpEntry.path +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = newDir + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")

        browserCommandQueue.moveRequest(fromPath, toPath)
        browserCommandQueue.directoryListingRequest(newDir, true)
        requestListReload(true)
        contextMenuDone()
    }

    function copyEntry(tmpEntry, newDir) {
        var fromPath = tmpEntry.path +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = newDir + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")

        browserCommandQueue.copyRequest(fromPath, toPath)
        browserCommandQueue.directoryListingRequest(newDir, true)
        contextMenuDone()
    }

    function deleteEntry(fullPath) {
        browserCommandQueue.removeRequest(fullPath);
        requestListReload(true)
        contextMenuDone()
    }

    Component {
        id: deleteRemorseComponent

        RemorseItem {
            id: remorseItem
            property string pathToResource : ""

            onCanceled: {
                requestListReload(true)
                remorseItem.destroy()
            }
            onTriggered: {
                deleteEntry(pathToResource)
                remorseItem.destroy()
            }
        }
    }

    function _renameEntry() {
        renameEntry(selectedEntry, dialogObj.text)
        _cleanup()
    }

    function _moveEntry() {
        moveEntry(selectedEntry, dialogObj.remotePath)
        _cleanup()
    }

    function _copyEntry() {
        copyEntry(selectedEntry, dialogObj.remotePath)
        _cleanup()
    }

    function _cleanup() {
        //dialogObj.destroy()
        dialogObj = null
        selectedEntry = null
    }

    Connections {
        target: transferQueue
        onCommandFinished: {
            __enableDestructiveMenus = !preventResourceModification(selectedEntry)
        }
    }

    MenuItem {
        id: renameMenuItem
        text: qsTr("Rename")
        enabled : __enableDestructiveMenus
        onClicked: {
            dialogObj = textEntryDialogComponent.createObject(pageRoot);
            dialogObj.text = selectedEntry.name
            dialogObj.acceptText = qsTr("Rename")
            dialogObj.placeholderText = qsTr("New name")
            dialogObj.labelText = dialogObj.placeholderText
            dialogObj.accepted.connect(_renameEntry);
            dialogObj.rejected.connect(_cleanup);
            pageStack.push(dialogObj);
        }
    }
    MenuItem {
        id: moveMenuItem
        text: qsTr("Move")
        enabled : __enableDestructiveMenus
        onClicked: {
            dialogObj = remoteDirDialogComponent.createObject(pageRoot, {entry: selectedEntry});
            dialogObj.browserCommandQueue = browserCommandQueue
            dialogObj.accepted.connect(_moveEntry);
            dialogObj.rejected.connect(_cleanup);
            pageStack.push(dialogObj);
        }
    }
    MenuItem {
        text: qsTr("Copy")
        onClicked: {
            dialogObj = remoteDirDialogComponent.createObject(pageRoot, {entry: selectedEntry});
            dialogObj.browserCommandQueue = browserCommandQueue
            dialogObj.accepted.connect(_copyEntry);
            dialogObj.rejected.connect(_cleanup);
            pageStack.push(dialogObj);
        }
    }
    MenuItem {
        id: deleteMenuItem
        text: qsTr("Delete")
        enabled : __enableDestructiveMenus
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

    MenuItem {
        id: detailsMenuItem
        text: qsTr("Details")
        onClicked: {
            var fileDetails = fileDetailsComponent.createObject(pageRoot, { entry: selectedEntry });
            pageStack.push(fileDetails);
        }
    }

}
