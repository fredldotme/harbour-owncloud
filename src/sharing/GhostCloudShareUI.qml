import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.thumbnailer 1.0
import Sailfish.TransferEngine 1.0
import com.github.beidl.harbourowncloud 1.0

ShareDialog {
    id: root
    canAccept: shouldShowList && !isLoadingDirectory && isValidAccount

    enabled: !isLoadingDirectory

    property int viewWidth: root.isPortrait ? Screen.width : Screen.width / 2
    property string remotePath : "/"
    property string localPath : root.source
    property var selectedAccount : permittedSettings
    readonly property bool isLoadingDirectory : webDavCommandQueue.running
    property bool errorOccured : false
    property bool thumbnailErrorOccured : false
    readonly property bool isValidAccount : (permittedSettings.username !== "" &&
                                             permittedSettings.password !== "")

    readonly property bool shouldShowList : (!errorOccured)

    readonly property string loginHint :
         "Please log in to your cloud"

    readonly property string errorHint :
         "An error occured while contacting the cloud instance"

    Component.onCompleted: {
        permittedSettings.readSettings()
    }

    FilePathUtil { id: filePathUtil }
    WebDavCommandQueue {
        id: webDavCommandQueue
        immediate: true
        settings: PermittedSettings {
            id: permittedSettings
            onSettingsChanged: {
                webDavCommandQueue.directoryListingRequest(remotePath, true);
            }
        }
        onCommandFinished: {
            if (!receipt.finished) {
                errorOccured = true;
                return;
            }

            var isDavListCommand = (receipt.info.property("type") === "davList")

            // Insert/update directory list in case of type === davList
            if (isDavListCommand) {
                var remotePath = receipt.info.property("remotePath")
                var dirContent = receipt.result.dirContent;
                directoryContents.insert(remotePath, dirContent);
                return;
            }
        }
    }
    QmlMap {
        id: directoryContents
        onInserted: {
            if (key !== remotePath)
                return;
            var dirContent = directoryContents.value(key);
            if (remotePath !== "/") {
                dirContent = [{ name: "..", isDirectory: true }].concat(dirContent)
            }

            listView.model = dirContent
        }
        onRemoved: console.log("REMOVED!")
    }

    onAccepted: {
        console.log("remotePath: " + remotePath);
        console.log("localPath: " + localPath);
        shareItem.start()
    }

    Thumbnail {
        id: thumbnail
        width: viewWidth
        height: parent.height / 2
        visible: shouldShowList
        source: root.source
        sourceSize.width: Screen.width
        sourceSize.height: Screen.height / 2
        onStatusChanged: {
            if (thumbnail.status === Thumbnail.Error) {
                thumbnailErrorOccured = true
            }
        }
    }
    ViewPlaceholder {
        enabled: thumbnailErrorOccured
        anchors.centerIn: thumbnail
        text: "No preview available"
    }

    Item {
        clip: true
        enabled: shouldShowList
        anchors.topMargin: 16

        anchors {
            top: root.isPortrait ? thumbnail.bottom : parent.top
            left: root.isPortrait ? parent.left: thumbnail.right
            right: parent.right
            bottom: parent.bottom
        }

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

                delegate: ListItem {
                    id: bgItem
                    contentHeight: Theme.itemSizeSmall
                    property var davInfo : listView.model[index]
                    visible: davInfo.isDirectory

                    Image {
                        id: icon
                        source: davInfo.name !== ".." ?
                                    "image://theme/icon-m-folder" :
                                    "image://theme/icon-m-back"
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
                        x: icon.x + icon.width + 6
                        y: icon.y - icon.height + 12
                        text: davInfo.name
                        visible: text !== ".."
                        anchors.verticalCenter: parent.verticalCenter
                        color: bgItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                        enabled: listView.enabled
                    }

                    onClicked: {
                        if (davInfo.isDirectory) {
                            var newTargetDir = filePathUtil.getCanonicalPath(remotePath + listView.model[index].name + "/");
                            remotePath = newTargetDir
                            webDavCommandQueue.directoryListingRequest(newTargetDir, true);
                        }
                    }
                }
            }

            ViewPlaceholder {
                id: placeholderHint
                anchors.centerIn: parent
                enabled: !shouldShowList || !isValidAccount
                text: loginHint
                MouseArea {
                    anchors.fill: parent
                    onClicked: permittedSettings.readSettings()
                }
            }
        }
    }

    SailfishShare {
        id: shareItem
        source: root.source
        metadataStripped: true
        serviceId: root.methodId
        userData: {
            "localPath" : localPath,
            "remoteDir" : remotePath,
            "hoststring" : selectedAccount.hoststring,
            "username" : selectedAccount.username,
            "password" : selectedAccount.password
        }
    }    

    DialogHeader {
    }
}

