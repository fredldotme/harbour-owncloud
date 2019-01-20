import QtQuick 2.0
import harbour.owncloud 1.0
import "qrc:/qml/ghostcloud/dialogs"

CommandPageFlow {
    id: pageFlowItemRoot
    property string targetRemotePath : "/"
    property var detailsStack : null

    // Pages
    readonly property Component browserComponent :
        Qt.createComponent("qrc:/qml/ghostcloud/pages/browser/FileBrowser.qml",
                           Component.PreferSynchronous);
    readonly property Component fileDetailsComponent :
        Qt.createComponent("qrc:/qml/ghostcloud/pages/browser/FileDetails.qml",
                           Component.PreferSynchronous);

    RemoteDirSelectDialog {
        id: copyEntryDialog
        browserCommandQueue: accountWorkers.browserCommandQueue
        directoryContents: pageFlow.directoryContents
        height: 400
    }

    RemoteDirSelectDialog {
        id: moveEntryDialog
        browserCommandQueue: accountWorkers.browserCommandQueue
        directoryContents: pageFlow.directoryContents
        height: 400
    }

    Connections {
        target: pageFlowItemRoot.accountWorkers.browserCommandQueue

        onCommandFinished: {
            console.log("onCommandFinished")

            if (!pageFlowItemRoot.enabled) {
                console.debug("disabled")
                return
            }

            var isDavListCommand = (receipt.info.property("type") === "davList")
            var isDavMoveCommand = (receipt.info.property("type") === "davMove")
            var isDavCopyCommand = (receipt.info.property("type") === "davCopy")
            var isDavRmCommand = (receipt.info.property("type") === "davRemove")

            // Ignore invalid CommandReceipts
            if (!receipt.valid) {
                console.debug("invalid receipt")
                return;
            }

            // Ignore if the command was intentionally aborted by the user
            if (receipt.abortIntended) {
                console.debug("abort intended")
                return
            }

            // Error messages
            if (!receipt.finished) {
                console.log("receipt not finished")
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
                console.log("list command")

                var remotePath = receipt.info.property("remotePath")
                var dirContent = receipt.result.dirContent;
                rootWindow.dirContents.insert(remotePath, dirContent);

                if (remotePath !== targetRemotePath) {
                    console.log("remotePath !== targetRemotePath")
                    return;
                }
                var isRefresh = receipt.info.property("refresh")

                console.log("New FileBrowser page, pageFlowItemRoot.directoryContents: " +
                            pageFlowItemRoot.directoryContents + ", rootWindow.dirContents: " +
                            rootWindow.dirContents)

                // Done in case of a refresh
                if (isRefresh) {
                    console.log("refresh done")
                    return;
                }

                // Complete pending PageStack animation
                /* if (pageStack.busy)
                    pageStack.completeAnimation()*/

                var nextDirectory = browserComponent.createObject(pageStack,
                                                                  {
                                                                      remotePath : remotePath,
                                                                      accountWorkers: accountWorkers,
                                                                      pageFlow: pageFlowItemRoot,
                                                                      copyEntryDialog: copyEntryDialog,
                                                                      moveEntryDialog: moveEntryDialog,
                                                                      detailsStack: detailsStack
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
                console.log("userInfo refresh")
                userInfoUpdateRequest()
                return
            }

            console.log("onCommandFinished done")
        }
    }
}
