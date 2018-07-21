import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import SailfishUiSet 1.0

Page {
    id: pageRoot
    //anchors.fill: parent

    property string remotePath : "/"
    property string pageHeaderText : "/"

    FileDetailsHelper { id: fileDetailsHelper }
    readonly property Component remoteDirDialogComponent :
        Qt.createComponent("qrc:/qml/pages/RemoteDirSelectDialog.qml");
    readonly property Component browserComponent :
        Qt.createComponent("qrc:/qml/pages/FileBrowser.qml");
    readonly property Component fileDetailsComponent :
        Qt.createComponent("qrc:/qml/pages/FileDetails.qml");
    readonly property Component selectionDialogComponent :
        Qt.createComponent("qrc:/sailfish-ui-set/ui/FileSelectionDialog.qml");
    readonly property Component textEntryDialogComponent :
        Qt.createComponent("qrc:/sailfish-ui-set/ui/TextEntryDialog.qml");

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

    property int cancelCounter : 0;

    Connections {
        target: transfer.uploadQueue
        onCommandFinished: {
            if (receipt.info.properties()["remotePath"] === pageRoot.remotePath) {
                refreshListView()
            }
        }
    }

    Connections {
        target: directoryContents
        onInserted: {
            console.log("key:" + key + " remotePath:" + remotePath)
            if (key !== remotePath)
                return;

            listView.model = directoryContents.value(key)
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Deactivating) {
            if (_navigation === PageNavigation.Back) {
                directoryContents.remove(remotePath)
            }
        }
    }

    property var selectedEntry : null;
    property BackgroundItem selectedItem : null;
    property RemorseItem selectedRemorse : null;
    property Dialog dialogObj : null

    function renameEntry(tmpEntry, newName) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = remotePath + newName +
                (tmpEntry.isDirectory ? "/" : "")

        var moveCommand = browser.moveRequest(fromPath, toPath, true)
        transfer.mainQueue.enqueue(moveCommand)
        var listCommand = browser.directoryListingRequest(remotePath)
        transfer.mainQueue.enqueue(listCommand)

        transfer.mainQueue.run()
    }

    function moveEntry(tmpEntry, remotePath) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = dialogObj.remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")

        var moveCommand = browser.moveRequest(fromPath, toPath, true)
        transfer.mainQueue.enqueue(moveCommand)
        var listCommand = browser.directoryListingRequest(remotePath)
        transfer.mainQueue.enqueue(listCommand)

        transfer.mainQueue.run()
    }

    function copyEntry(tmpEntry, remotePath) {
        var fromPath = remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var toPath = dialogObj.remotePath + tmpEntry.name +
                (tmpEntry.isDirectory ? "/" : "")
        var command = browser.copyRequest(fromPath, toPath, true)

        transfer.mainQueue.enqueue(command)
        transfer.mainQueue.run()
    }

    SilicaFlickable {
        anchors.fill: parent

        SilicaListView {
            id: listView
            anchors.fill: parent
            clip: true

            header: PageHeader {
                id: pageHeader
                title: pageHeaderText
            }

            BusyIndicator {
                anchors.right: listView.header.left
                anchors.top: listView.header.top
                anchors.bottom: listView.header.bottom
                width: height
                running: transfer.mainQueue.running
            }

            PullDownMenu {
                MenuItem {
                    text: qsTr("Refresh")
                    enabled: listView.model !== undefined
                    onClicked: {
                        var command = browser.directoryListingRequest(remotePath);
                        transfer.mainQueue.enqueue(command)
                        transfer.mainQueue.run()
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
                            var commandEntity = browser.makeDirectoryRequest(dialogObj.text)
                            transfer.mainQueue.enqueue(commandEntity)
                            transfer.mainQueue.run()
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
                        dialogObj = selectionDialogComponent.createObject(pageRoot,
                                                                          {maximumSelections:Number.MAX_VALUE});
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
                        pageStack.push("qrc:/qml/pages/TransferPage.qml")
                    }
                }

                MenuItem {
                    text: qsTr("Settings")
                    onClicked: {
                        pageStack.push("qrc:/qml/pages/SettingsPage.qml")
                    }
                }
            }

            delegate: ListItem {
                id: delegate

                property var davInfo : listView.model[index]

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
                        var nextDirectory = browserComponent.createObject(pageRoot, { remotePath : nextPath });
                        pageStack.push(nextDirectory)
                        var command = browser.directoryListingRequest(nextPath)
                        transfer.mainQueue.enqueue(command)
                        transfer.mainQueue.run()
                    } else {
                        var fileDetails = fileDetailsComponent.createObject(pageRoot, {entry: davInfo});
                        pageStack.push(fileDetails);
                    }
                }
                onPressAndHold: {
                    selectedEntry = davInfo;
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
                size: BusyIndicatorSize.Large
            }

            Component {
                id: contextMenuComponent

                ContextMenu {
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
                            selectedRemorse = remorseItem;
                            tmpEntry = selectedEntry
                            cancelCounter++;
                            selectedRemorse.execute(selectedItem,
                                                    qsTr("Deleting", "RemorseItem text"), function() {
                                cancelCounter--;
                                var command = browser.removeRequest(remotePath + tmpEntry.name +
                                                                    (tmpEntry.isDirectory ? "/" : ""),
                                                                    cancelCounter == 0);
                                transfer.mainQueue.enqueue(command)
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





