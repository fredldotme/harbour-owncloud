import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Dialog {
    id: dialogRoot
    canAccept: !isLoadingDirectory

    property string remotePath : "/"
    property bool isLoadingDirectory : true

    Component.onCompleted: {
        tmpBrowser.settings.readSettings()
        tmpBrowser.testConnection()
    }

    OwnCloudBrowser {
        id: tmpBrowser
        settings: browser.settings

        onLoginFailed: {
            dialogRoot.close()
        }

        onSslCertifcateError: {
            dialogRoot.close()
        }

        onDirectoryContentChanged: {
            var dirOnlyEntries = [];
            remotePath = currentPath

            if (remotePath !== "/") {
                dirOnlyEntries.push( { name : "..", isDirectory: true} );
            }

            for (var i = 0; i < entries.length; i++) {
                if (entries[i].isDirectory)
                    dirOnlyEntries.push(entries[i])
            }
            listView.model = dirOnlyEntries;
            isLoadingDirectory = false;
        }
        onLoginSucceeded: {
            tmpBrowser.getDirectoryContent("/");
        }
    }

    Item {
        clip: true

        anchors.top: dialogHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        SilicaFlickable {
            anchors.fill: parent

            BusyIndicator {
                anchors.centerIn: parent
                running: isLoadingDirectory
            }

            SilicaListView {
                id: listView
                anchors.fill: parent
                enabled: !isLoadingDirectory

                delegate: BackgroundItem {
                    id: bgItem
                    Image {
                        id: icon
                        source: listView.model[index].name !== ".." ?
                                    "image://theme/icon-m-folder" :
                                    "image://theme/icon-m-back"
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
                            var newTargetDir = tmpBrowser.getCanonicalPath(remotePath + listView.model[index].name + "/");
                            tmpBrowser.getDirectoryContent(newTargetDir);
                            isLoadingDirectory = true;
                        }
                    }
                }
            }
        }
    }

    DialogHeader {
        id: dialogHeader
    }
}

