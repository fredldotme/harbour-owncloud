import QtQuick 2.0
import harbour.owncloud 1.0
import SailfishUiSet 1.0

CommandPageFlow {
    id: pageFlowItemRoot
    property string targetRemotePath : "/"

    FileDetailsHelper { id: fileDetailsHelper }

    // Open files conditionally after download
    Connections {
        target: accountWorkers.transferCommandQueue
        onCommandFinished: {
            console.log("transfer command finished")
            const isFileDownload = (receipt.info.property("type") === "fileDownload")
            if (!isFileDownload)
                return;

            const fileOpenRequested = receipt.info.property("fileOpen");
            const fileDestination = receipt.info.property("localPath");
            if (fileOpenRequested) {
                Qt.openUrlExternally("file://" + fileDestination)
            }
        }
    }

    Connections {
        target: accountWorkers.browserCommandQueue
        onCommandFinished: {
            console.log("onCommandFinished")
            var isDavListCommand = (receipt.info.property("type") === "davList")
            var isDavMoveCommand = (receipt.info.property("type") === "davMove")
            var isDavCopyCommand = (receipt.info.property("type") === "davCopy")
            var isDavRmCommand = (receipt.info.property("type") === "davRemove")

            // Ignore invalid CommandReceipts
            if (!receipt.valid) {
                console.warn("Receipt invalid")
                return;
            }

            // Ignore if the command was intentionally aborted by the user
            if (receipt.abortIntended) {
                console.warn("Abort intended")
                return
            }

            // Error messages
            if (!receipt.finished) {
                console.warn("Receipt: unfinished")
                if (isDavListCommand) {
                    notificationRequest(
                                qsTr("Failed to get remote content"),
                                qsTr("Please check your connection or try again later."))
                    return
                }
                if (isDavRmCommand || isDavMoveCommand || isDavCopyCommand) {
                    notificationRequest(
                                qsTr("Operation failed"),
                                qsTr("Please check your connection or try again later."))
                    return
                }

                return;
            }

            // Insert/update directory list in case of type === davList and push
            // a new page into the PageStack in case a refresh was not requested
            if (isDavListCommand) {
                var remotePath = receipt.info.property("remotePath")
                var isRefresh = receipt.info.property("refresh")
                var dirContent = receipt.result.dirContent;

                directoryContents.insert(remotePath, dirContent);

                if (remotePath !== targetRemotePath) {
                    console.log("remotePath !== targetRemotePath")
                    return;
                }

                // Done in case of a refresh
                if (isRefresh)
                    return;

                // TODO: investigate
                if (!receipt.result.success) {
                    notificationRequest(
                                qsTr("Error occured"),
                                qsTr("Please check your credentials or try again later."))
                    return
                }

                // Complete pending PageStack animation
                if (pageStack.busy)
                    pageStack.completeAnimation()

                var nextDirectory = browserComponent.createObject(pageStack,
                                                                  {
                                                                      remotePath : remotePath,
                                                                      accountWorkers: accountWorkers,
                                                                      pageFlow: pageFlowItemRoot
                                                                  });

                if (!nextDirectory) {
                    console.warn(browserComponent.errorString())
                    return
                }

                nextDirectory.transientNotification.connect(transientNotificationRequest)

                // Try to fetch the avatar which
                // will only succeed if the server supports it.
                if (remotePath === "/") {
                    userInfoUpdateRequest()
                    avatarFetchRequest()
                }
                pageStack.push(nextDirectory)

                return
            }

            // Refresh userInfo after remote removal or copy
            if (isDavRmCommand || isDavCopyCommand) {
                userInfoUpdateRequest()
                return
            }
        }
    }
}
