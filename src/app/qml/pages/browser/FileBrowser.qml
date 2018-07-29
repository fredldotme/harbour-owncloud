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

    function refreshListView() {
        listCommand = browserCommandQueue.directoryListingRequest(remotePath)
        browserCommandQueue.run()
    }

    FileDetailsHelper { id: fileDetailsHelper }
    Notification {
        id: transientNotifier
        isTransient: true
    }

    Component.onCompleted: {
        refreshListView()
    }

    onStatusChanged: {
        if (status === PageStatus.Inactive) {
            if (_navigation !== undefined && _navigation === PageNavigation.Back) {
                console.debug("cleanup")
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
                refreshListView()
                return;
            }
        }
    }

    Connections {
        target: transferQueue
        onCommandFinished: {
            if (receipt.info.property("remotePath") === pageRoot.remotePath) {
                refreshListView()
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
        refreshListView()
    }

    function moveEntry(tmpEntry, remotePath) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = dialogObj.remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")

        browserCommandQueue.moveRequest(fromPath, toPath, true)
        refreshListView()
    }

    function copyEntry(tmpEntry, remotePath) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = dialogObj.remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")

        browserCommandQueue.copyRequest(fromPath, toPath, true)
        browserCommandQueue.run()
    }

    SilicaFlickable {
        anchors.fill: parent

        SilicaListView {
            id: listView
            anchors.fill: parent
            clip: true
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
                        refreshListView()
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
                            refreshListView()
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
                            console.debug("lastModified: " + selectedFiles[i].lastModified);
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
                RemorseItem {
                    id: remorseItem
                    onCanceled: {
                        console.log("Canceled...")
                        refreshListView()
                    }
                }

                onClicked: {
                    if(davInfo.isDirectory) {
                        var nextPath = remotePath + davInfo.name + "/";
                        var nextDirectory = browserComponent.createObject(pageRoot, { remotePath : nextPath });
                        pageStack.push(nextDirectory)
                    } else {
                        var fileDetails = fileDetailsComponent.createObject(pageRoot, { entry: davInfo });
                        pageStack.push(fileDetails);
                    }
                }
                onPressAndHold: {
                    selectedEntry = davInfo;
                    selectedItem = delegate
                    if (!menu)
                        menu = contextMenuComponent.createObject(listView, {remorseItem : remorseItem})

                    if (remorseItem.pending === null) {
                        openMenu()
                    }
                }
            }
            VerticalScrollDecorator {}

            BusyIndicator {
                anchors.centerIn: parent
                running: listCommand !== null
                size: BusyIndicatorSize.Large
            }

            Component {
                id: contextMenuComponent

                ContextMenu {
                    property RemorseItem remorseItem : null
                    onClosed: {
                        selectedEntry = null
                        selectedItem = null
                    }
                    MenuItem {
                        property var tmpEntry;

                        function renameSelectedEntry() {
                            renameEntry(tmpEntry, dialogObj.text)
                            dialogObj = null
                            tmpEntry = null
                        }

                        text: qsTr("Rename")
                        onClicked: {
                            tmpEntry = selectedEntry
                            dialogObj = textEntryDialogComponent.createObject(pageRoot);
                            dialogObj.text = tmpEntry.name
                            dialogObj.acceptText = qsTr("Rename")
                            dialogObj.placeholderText = qsTr("New name")
                            dialogObj.labelText = dialogObj.placeholderText
                            dialogObj.accepted.connect(renameSelectedEntry);
                            dialogObj.rejected.connect(function() {
                                dialogObj = null
                                tmpEntry = null
                            });
                            pageStack.push(dialogObj);
                        }
                    }
                    MenuItem {
                        property var tmpEntry;

                        function moveSelectedEntry() {
                            moveEntry(tmpEntry, remotePath)
                            dialogObj = null
                            tmpEntry = null
                        }

                        text: qsTr("Move")
                        onClicked: {
                            tmpEntry = selectedEntry
                            dialogObj = remoteDirDialogComponent.createObject(pageRoot, {entry: tmpEntry});
                            dialogObj.accepted.connect(moveSelectedEntry);
                            dialogObj.rejected.connect(function() {
                                dialogObj = null
                                tmpEntry = null
                            });
                            pageStack.push(dialogObj);
                        }
                    }
                    MenuItem {
                        property var tmpEntry;

                        function copySelectedEntry() {
                            copyEntry(tmpEntry, remotePath)
                            dialogObj = null
                            tmpEntry = null
                        }

                        text: qsTr("Copy")
                        onClicked: {
                            tmpEntry = selectedEntry
                            dialogObj = remoteDirDialogComponent.createObject(pageRoot, {entry: tmpEntry});
                            dialogObj.accepted.connect(copySelectedEntry);
                            dialogObj.rejected.connect(function() {
                                dialogObj = null
                                tmpEntry = null
                            });
                            pageStack.push(dialogObj);
                        }
                    }
                    MenuItem {
                        property var tmpEntry;
                        text: qsTr("Delete")
                        onClicked: {
                            tmpEntry = selectedEntry
                            remorseItem.execute(selectedItem,
                                                qsTr("Deleting",
                                                     "RemorseItem text"),
                                                function(){
                                                    browserCommandQueue.removeRequest(remotePath + tmpEntry.name);
                                                    refreshListView()
                                                })
                        }
                    }
                }
            }
        }
    }
}





