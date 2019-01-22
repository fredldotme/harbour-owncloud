import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import harbour.owncloud 1.0

Dialog {
    id: dialogRoot

    property string remotePath : "/"
    property bool isLoadingDirectory : true
    property CloudStorageProvider browserCommandQueue : null
    property QmlMap directoryContents : rootWindow.dirContents

    // Prepend ".." in case remotePath !== "/"
    function mangledDirectoryList(dirs) {
        if (remotePath === "/")
            return dirs;
        return [{ name: "..", isDirectory: true }].concat(dirs)
    }

    function getDirectoryContent(path) {
        remotePath = FilePathUtil.getCanonicalPath(path)

        if (dialogRoot.directoryContents.contains(remotePath)) {
            listView.model = mangledDirectoryList(dialogRoot.directoryContents.value(remotePath))
            isLoadingDirectory = false;
            return;
        }
        dialogRoot.browserCommandQueue.directoryListingRequest(remotePath, true)
    }

    Component.onCompleted: {
        getDirectoryContent(remotePath)

        console.debug("dialogRoot.directoryContents " + dialogRoot.directoryContents);
    }

    Connections {
        target: dialogRoot.directoryContents

        onInserted: {
            console.log("RemoteDirSelectDialog key:" + key + " remotePath:" + remotePath)
            if (key !== remotePath)
                return;

            listView.model = mangledDirectoryList(dialogRoot.directoryContents.value(key))
            isLoadingDirectory = false;
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        enabled: !isLoadingDirectory

        delegate: Button {
            id: icon
            visible: davInfo.isDirectory
            background: Rectangle { color: "transparent" }
            icon.color: "transparent"
            icon.name: "folder"
            text: davInfo.name
            //anchors.verticalCenter: parent.verticalCenter

            property var davInfo : listView.model[index]

            onClicked: {
                if(!davInfo.isDirectory)
                    return;

                var newTargetDir = FilePathUtil.getCanonicalPath(remotePath + "/" +
                                                                 davInfo.name + "/")
                console.log(newTargetDir)
                isLoadingDirectory = true;
                getDirectoryContent(newTargetDir)
            }
        }

        BusyIndicator {
            anchors.centerIn: parent
            running: isLoadingDirectory
        }
    }
}
