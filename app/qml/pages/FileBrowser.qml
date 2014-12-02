import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0


Page {
    id: pageRoot
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

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("Settings")
                onClicked: {
                    pageStack.push("SettingsPage.qml")
                }
            }

            MenuItem {
                text: qsTr("File transfers")
                onClicked: {
                    pageStack.push("TransferPage.qml")
                }
            }

            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    listView.model = undefined
                    browser.getDirectoryContent(remotePath);
                }
            }
        }

        SilicaListView {
            id: listView
            anchors.fill: parent
            header: PageHeader {
                title: remotePath
            }
            delegate: BackgroundItem {
                id: delegate

                Image {
                    id: icon
                    source: listView.model[index].isDirectory ?
                                "../images/large-folder.png" :
                                "../images/large-file.png"
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.top: parent.top
                    anchors.topMargin: 18
                    height: label.height
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: label
                    x: icon.x + icon.width
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
            }
            VerticalScrollDecorator {}

            BusyIndicator {
                anchors.centerIn: parent
                running: listView.model === undefined
            }
        }
    }
}





