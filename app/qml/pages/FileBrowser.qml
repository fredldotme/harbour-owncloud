import QtQuick 2.0
import Sailfish.Silica 1.0
import OwnCloud 1.0


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
                console.log(entries);
                console.log(entries.length);
                listView.model = entries;
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
                source: "../images/large-folder.png"
                height: label.height
                width: height
            }

            Label {
                id: label
                x: icon.x + icon.width
                y: icon.y - icon.height
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
                    var fileDetails = fileComponent.createObject(pageRoot);
                    fileDetails.filePath = remotePath;
                    fileDetails.entry = listView.model[index];
                    pageStack.push(fileDetails);
                }
            }
        }
        VerticalScrollDecorator {}
    }
}





