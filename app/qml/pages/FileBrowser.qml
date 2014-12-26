import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0


Page {
    id: pageRoot
    anchors.fill: parent

    property string remotePath : "/"

    Component.onCompleted: {
        remotePath = browser.getCurrentPath();
    }

    Connections {
        target: browser
        onDirectoryContentChanged: {
            if(currentPath == remotePath) {
                listView.model = entries;
            }
        }
    }

    Connections {
        target: browser
        onRefreshStarted: {
            if(remotePath === pathToRefresh)
                listView.model = undefined;
        }
    }

    Connections {
        target: transfer
        onUploadComplete: {
            if(remotePath === pageRoot.remotePath) {
                listView.model = undefined
                browser.getDirectoryContent(remotePath);
            }
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Deactivating) {
            if (_navigation === PageNavigation.Back) {
                browser.goToParentPath();
            }
        }
    }

    property Item fileContextMenu;
    property EntryInfo selectedEntry;
    property BackgroundItem selectedItem;

    SilicaFlickable {
        anchors.fill: parent

        SilicaListView {
            id: listView
            anchors.fill: parent

            header: PageHeader {
                title: remotePath
            }

            PullDownMenu {
                MenuItem {
                    text: qsTr("Refresh")
                    onClicked: {
                        listView.model = undefined
                        browser.getDirectoryContent(remotePath);
                    }
                }

                MenuItem {
                    text:qsTr("Make directory")
                    onClicked: {
                        pageStack.push("MkDirDialog.qml")
                    }
                }

                MenuItem {
                    text: qsTr("Upload")
                    onClicked: {
                        pageStack.push("UploadDialog.qml")
                    }
                }
            }
            PushUpMenu {
                MenuItem {
                    text: qsTr("File transfers")
                    enabled: transfer.transfering
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

            delegate: BackgroundItem {
                id: delegate
                property bool menuOpen: fileContextMenu != null && fileContextMenu.parent === delegate
                height: menuOpen ? fileContextMenu.height + bgItem.height : bgItem.height

                BackgroundItem {
                    id: bgItem
                    Image {
                        id: icon
                        source: listView.model[index].isDirectory ?
                                    "image://theme/icon-m-folder" :
                                    getIconFromMime(listView.model[index].mimeType)
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.paddingLarge
                        anchors.top: parent.top
                        anchors.topMargin: 18
                        height: label.height
                        fillMode: Image.PreserveAspectFit
                    }

                    Label {
                        id: label
                        x: icon.x + icon.width + 6
                        y: icon.y - icon.height + 6
                        text: listView.model[index].name
                        anchors.verticalCenter: parent.verticalCenter
                        color: bgItem.highlighted ? Theme.highlightColor : Theme.primaryColor
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
                        if (!fileContextMenu)
                            fileContextMenu = contextMenuComponent.createObject(listView)
                        fileContextMenu.show(delegate)
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
                        text: "Delete"

                        property string fileName;

                        onClicked: {
                            fileName = selectedEntry.name
                            remorseItem.execute(selectedItem, "Deleting", function() {
                                browser.remove(fileName)
                            })
                        }
                    }
                }
            }

            RemorseItem { id: remorseItem }
        }
    }
}





