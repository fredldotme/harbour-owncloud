import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import Nemo.Notifications 1.0

ApplicationWindow
{
    DaemonControl {
        id: daemonCtrl
    }
    Notification {
        id: notifier
    }

    readonly property bool isTransfering :
        (transferQueue.running || daemonCtrl.uploading)

    readonly property Component remoteDirDialogComponent :
        Qt.createComponent("qrc:/qml/pages/browser/RemoteDirSelectDialog.qml");
    readonly property Component browserComponent :
        Qt.createComponent("qrc:/qml/pages/browser/FileBrowser.qml");
    readonly property Component fileDetailsComponent :
        Qt.createComponent("qrc:/qml/pages/browser/FileDetails.qml");
    readonly property Component selectionDialogComponent :
        Qt.createComponent("qrc:/sailfish-ui-set/ui/FileSelectionDialog.qml");
    readonly property Component textEntryDialogComponent :
        Qt.createComponent("qrc:/sailfish-ui-set/ui/TextEntryDialog.qml");
    readonly property Component authenticationEntranceComponent :
        Qt.createComponent("qrc:/qml/pages/login/Entrance.qml");
    readonly property Component basicAuthenticationComponent :
        Qt.createComponent("qrc:/qml/pages/login/BasicAuthentication.qml");
    readonly property Component flowAuthenticationComponent :
        Qt.createComponent("qrc:/qml/pages/login/FlowAuthentication.qml");
    readonly property Component sslErrorDialogComponent :
        Qt.createComponent("qrc:/qml/pages/login/SSLErrorDialog.qml")
    readonly property Component settingsPageComponent :
        Qt.createComponent("qrc:/qml/pages/SettingsPage.qml")
    readonly property Component transferPageComponent :
        Qt.createComponent("qrc:/qml/pages/TransferPage.qml")


    function refreshUserInfo() {
        // Skip in case there's already a userInfo command in the queue
        for (var i = 0; i < ocsCommandQueue.queue.length; i++) {
            var commandInfo = ocsCommandQueue.queue[i].info;
            if (commandInfo.type === "userInfo")
                return;
        }

        if (transferQueue.queue.length < 1) {
            ocsCommandQueue.userInfoRequest();
            ocsCommandQueue.run()
        }
    }

    WebDavCommandQueue {
        id: browserCommandQueue
        settings: persistentSettings
        onCommandFinished: {
            console.log("onCommandFinished")
            if (!receipt.finished)
                return;

            var isDavListCommand = (receipt.info.property("type") === "davList")
            var isDavRmCommand = (receipt.info.property("type") === "davRemove")
            var isDavCopyCommand = (receipt.info.property("type") === "davCopy")

            // Insert/update directory list in case of type === davList
            if (isDavListCommand) {
                var remotePath = receipt.info.property("remotePath")
                var dirContent = receipt.result;
                directoryContents.insert(remotePath, dirContent);
                return;
            }

            // Refresh userInfo after remote removal or copy
            if (isDavRmCommand || isDavCopyCommand) {
                refreshUserInfo()
                return;
            }

        }
    }
    WebDavCommandQueue {
        id: transferQueue
        settings: persistentSettings
        onCommandFinished: {
            var transferType = receipt.info.property("type")
            console.log("transferType " + transferType)
            var fileName = receipt.info.property("fileName")

            // Notify the user of successful or erroneous file transfers.
            // Also refresh the userInfo in case an upload was successful.
            if (transferType === "fileUpload") {
                if (receipt.finished) {
                    notify(qsTr("Upload complete"), qsTr("%1 uploaded successfully").arg(fileName))
                    refreshUserInfo()
                } else {
                    notify(qsTr("Upload failed!"), qsTr("%1 couldn't be uploaded").arg(fileName))
                }
            } else if (transferType === "fileDownload") {
                if (receipt.finished)
                    notify(qsTr("Download complete"), qsTr("%1 downloaded successfully").arg(fileName))
                else
                    notify(qsTr("Download failed!"), qsTr("%1 couldn't be downloaded").arg(fileName))
            }
        }
    }
    OcsCommandQueue {
        id: ocsCommandQueue
        settings: persistentSettings
    }

    function isTransferEnqueued(remoteFilePath) {
        for(var i = 0; i < transferQueue.queue.length; i++) {
            var info = transferQueue.queue[i].info;
            if (info.property("remoteFile") === remoteFilePath) {
                return true;
            }
        }
        return false;
    }

    function notify(summary, body) {
        if (!persistentSettings.notifications)
            return;

        notifier.summary = summary
        notifier.previewSummary = summary
        notifier.body = body
        notifier.previewBody = body
        notifier.publish();
    }

    /*Connections {
        target: browserCommandQueue
        onSslCertifcateError: {
            pageStack.completeAnimation();
            pageStack.clear();
            pageStack.replace(authenticationEntranceComponent, {}, PageStackAction.Immediate)
            pageStack.push("qrc:/qml/pages/login/SSLErrorDialog.qml", {md5Digest : md5Digest, sha1Digest : sha1Digest});
        }
    }*/

    /*Connections {
        target: browser
        onLoginFailed: {
            loginInProgress = false;
            loginFailed = true;
            notify(qsTr("Login failed"), qsTr("Please check your host address and credentials"))

            pageStack.completeAnimation();
            pageStack.clear();
            pageStack.completeAnimation();
            pageStack.push(authenticationEntranceComponent)
        }
    }*/

    QmlMap {
        id: directoryContents
        onInserted: console.log("INSERTED!")
        onRemoved: console.log("REMOVED!")
    }

    id: applicationWindow
    initialPage: basicAuthenticationComponent
    cover: Qt.resolvedUrl("qrc:/qml/cover/CoverPage.qml")
}


