import QtQuick 2.0
import Sailfish.Silica 1.0


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

            Label {
                x: Theme.paddingLarge
                text: listView.model[index].name
                anchors.verticalCenter: parent.verticalCenter
                color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
            }

            onClicked: {
                var nextPage = Qt.createComponent("FileBrowser.qml");
                browser.getDirectoryContent(remotePath + listView.model[index].name + "/");
                pageStack.push(nextPage)
            }
        }
        VerticalScrollDecorator {}
    }
}





