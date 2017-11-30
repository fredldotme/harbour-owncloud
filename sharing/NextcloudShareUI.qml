import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.thumbnailer 1.0
import Sailfish.TransferEngine 1.0
import com.github.beidl.harbourowncloud 1.0

ShareDialog {
    id: root
    canAccept: shouldShowList && !isLoadingDirectory

    enabled: !isLoadingDirectory

    property int viewWidth: root.isPortrait ? Screen.width : Screen.width / 2
    property string remotePath : "/"
    property string localPath : root.source
    property bool isLoadingDirectory : (browser.settings.autoLogin)
    property bool errorOccured : false
    property bool thumbnailErrorOccured : false

    readonly property bool shouldShowList : (browser.settings.autoLogin && !errorOccured)

    readonly property string loginHint :
         "Please log in to your Nextcloud/ownCloud and enable automatic login"

    readonly property string errorHint :
         "An error occured while contacting the Nextcloud/ownCloud instance"

    Component.onCompleted: {
        browser.settings.readSettings()
        if (browser.settings.autoLogin)
            browser.testConnection()
    }

    OwnCloudBrowser {
        id: browser

        onLoginFailed: {
            placeholderHint.text = errorHint
            errorOccured = true
            isLoadingDirectory = false
        }

        onSslCertifcateError: {
            placeholderHint.text = errorHint
            errorOccured = true
            isLoadingDirectory = false
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
            if (shouldShowList)
                browser.getDirectoryContent("/");
        }
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
                            var newTargetDir = browser.getCanonicalPath(remotePath + listView.model[index].name + "/");
                            browser.getDirectoryContent(newTargetDir);
                            isLoadingDirectory = true;
                        }
                    }
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
                    "remoteDir" : remotePath
        }
    }    

    ViewPlaceholder {
        id: placeholderHint
        anchors.centerIn: parent
        enabled: !shouldShowList
        text: loginHint
    }

    DialogHeader {
    }
}

