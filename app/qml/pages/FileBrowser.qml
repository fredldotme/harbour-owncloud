import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import SailfishUiSet 1.0

Page {
    id: pageRoot
    anchors.fill: parent

    property string remotePath : "/"
    property string pageHeaderText : "/"

    FileDetailsHelper { id: fileDetailsHelper }
    readonly property Component remoteDirDialogComponent : Qt.createComponent("RemoteDirSelectDialog.qml");
    readonly property Component selectionDialogComponent : Qt.createComponent("qrc:/sailfish-ui-set/ui/FileSelectionDialog.qml");
    readonly property Component textEntryDialogComponent : Qt.createComponent("qrc:/sailfish-ui-set/ui/TextEntryDialog.qml");

    Component.onCompleted: {
        remotePath = browser.getCurrentPath();
    }

    onRemotePathChanged: {
        if(remotePath === "/") {
            pageHeaderText = "/";
        } else {
            var dirs = remotePath.split("/")
            if(dirs.length > 1) {
                pageHeaderText = dirs[dirs.length - 2]
            }
        }
    }

    property int cancelCounter : 0;

    function refreshListView() {
        listView.model = undefined
        browser.getDirectoryContent(remotePath);
    }

    Connections {
        id: fillListviewConnection
        target: browser
        onDirectoryContentChanged: {
            if(currentPath === browser.getCanonicalPath(remotePath)) {
                listView.model = entries;
            }
        }
    }

    Connections {
        target: browser
        onRefreshStarted: {
            if(remotePath === pathToRefresh && cancelCounter === 0) {
                listView.model = undefined;
            }
        }
    }

    Connections {
        target: transfer
        onUploadComplete: {
            /// XXX: Would like to pass `entry` in the signal, but it confuses navigation!
            //remotePath = entry.getRemotePath();
            if(remotePath === pageRoot.remotePath) {
                refreshListView()
            }
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Deactivating) {
            if (_navigation === PageNavigation.Back) {
                fillListviewConnection.destroy()
                listView.model = undefined
                browser.goToParentPath();
            }
        }
    }

    property EntryInfo selectedEntry : null;
    property BackgroundItem selectedItem : null;
    property RemorseItem selectedRemorse : null;
    property Dialog dialogObj : null

    function renameEntry(tmpEntry, newName) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = remotePath + newName +
                (tmpEntry.isDirectory ? "/" : "")
        browser.move(fromPath, toPath, true)
    }

    function moveEntry(tmpEntry, remotePath) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = dialogObj.remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        browser.move(fromPath, toPath, true)
    }

    function copyEntry(tmpEntry, remotePath) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = dialogObj.remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        browser.copy(fromPath, toPath, true)
    }

    SilicaFlickable {
        anchors.fill: parent

        SilicaListView {
            id: listView
            anchors.fill: parent
            clip: true

            header: PageHeader {
                title: pageHeaderText
            }

            PullDownMenu {
                MenuItem {
                    text: qsTr("Refresh")
                    enabled: listView.model !== undefined
                    onClicked: {
                        listView.model = undefined
                        browser.getDirectoryContent(remotePath);
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
                            browser.makeDirectory(dialogObj.text)
                            dialogObj = null
                        })
                        pageStack.push(dialogObj)
                    }
                }

                MenuItem {
                    function enqueueSelectedFiles() {
                        var selectedFiles = dialogObj.filesToSelect
                        for (var i = 0; i < selectedFiles.length; i++) {
                            transfer.enqueueUpload(selectedFiles[i], browser.getCurrentPath());
                        }
                        dialogObj = null
                    }

                    text: qsTr("Upload")
                    enabled: listView.model !== undefined
                    onClicked: {
                        dialogObj = selectionDialogComponent.createObject(pageRoot, {maximumSelections:Number.MAX_VALUE});
                        dialogObj.acceptText = qsTr("Upload")
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
                        pageStack.push("TransferPage.qml")
                    }
                }

                MenuItem {
                    text: qsTr("Settings")
                    onClicked: {
                        pageStack.push("SettingsPage.qml")
                    }
                }
            }

            delegate: ListItem {
                id: delegate

                RemorseItem {
                    id: remorseItem
                    onCanceled: {
                        console.log("Canceled...")
                        var forceRefresh = (cancelCounter > 0);
                        cancelCounter--;
                        if(forceRefresh) {
                            refreshListView()
                        }
                    }
                }

                Image {
                    id: icon
                    source: listView.model[index].isDirectory ?
                                "image://theme/icon-m-folder" :
                                fileDetailsHelper.getIconFromMime(listView.model[index].mimeType)
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
                    text: listView.model[index].name
                    anchors.verticalCenter: parent.verticalCenter
                    color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                }

                onClicked: {
                    if(listView.model[index].isDirectory) {
                        var nextDirectory = Qt.createComponent("FileBrowser.qml");
                        browser.getDirectoryContent(remotePath + listView.model[index].name + "/");
                        pageStack.push(nextDirectory)
                    } else {
                        var fileComponent = Qt.createComponent("FileDetails.qml");
                        var fileDetails = fileComponent.createObject(pageRoot, {entry: listView.model[index]});
                        pageStack.push(fileDetails);
                    }
                }
                onPressAndHold: {
                    selectedEntry = listView.model[index];
                    selectedItem = delegate
                    if (!menu)
                        menu = contextMenuComponent.createObject(listView)

                    if (selectedRemorse === null) {
                        openMenu()
                    }
                }
            }
            VerticalScrollDecorator {}

            BusyIndicator {
                anchors.centerIn: parent
                running: listView.model === undefined
            }

            Component {
                id: contextMenuComponent

                ContextMenu {
                    onClosed: {
                        selectedEntry = null
                        selectedItem = null
                    }
                    MenuItem {
                        property EntryInfo tmpEntry;

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
                        property EntryInfo tmpEntry;

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
                        property EntryInfo tmpEntry;

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
                        property EntryInfo tmpEntry;
                        text: qsTr("Delete")
                        onClicked: {
                            selectedRemorse = remorseItem;
                            tmpEntry = selectedEntry
                            cancelCounter++;
                            selectedRemorse.execute(selectedItem, qsTr("Deleting", "RemorseItem text"), function() {
                                cancelCounter--;
                                browser.remove(remotePath + tmpEntry.name +
                                               (tmpEntry.isDirectory ? "/" : ""),
                                               cancelCounter == 0);
                                selectedRemorse = null;
                            })
                            selectedRemorse.canceled.connect(function() {
                                selectedRemorse = null
                            })
                        }
                    }
                }
            }
        }
    }
}





