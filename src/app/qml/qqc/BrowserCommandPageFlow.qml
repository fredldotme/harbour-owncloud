import QtQuick 2.0
import QtQuick.Controls 2.2
import harbour.owncloud 1.0
import QmlUiSet 1.0
import "qrc:/qml/qqc"
import "qrc:/qml/qqc/controls"
import "qrc:/qml/qqc/dialogs"
import "qrc:/qml-ui-set"

CommandPageFlow {
    id: pageFlowItemRoot
    property string targetRemotePath : "/"
    property var detailsStack : null
    property alias userInfo : userInfo

    // Pages
    readonly property Component browserComponent :
        Qt.createComponent("qrc:/qml/qqc/pages/browser/FileBrowser.qml",
                           Component.PreferSynchronous);
    readonly property Component fileDetailsComponent :
        Qt.createComponent("qrc:/qml/qqc/pages/browser/FileDetails.qml",
                           Component.PreferSynchronous);
    readonly property Component contentHubOpenerComponent :
        Qt.createComponent("qrc:/qml/qqc/dialogs/fileselect/UbuntuContentHubOpener.qml",
                           Component.PreferSynchronous);

    FileDetailsHelper { id: fileDetailsHelper }

    Connections {
        target: accountWorkers.accountInfoCommandQueue
        onCommandFinished: {
            console.debug("finished, account info enabled? " + receipt.result.enabled)

            userInfo.enabled = receipt.result.enabled
            userInfo.displayName = receipt.result.displayName
            userInfo.email = receipt.result.email
            userInfo.freeBytes = fileDetailsHelper.getHRSize(receipt.result.freeBytes)
            userInfo.usedBytes = fileDetailsHelper.getHRSize(receipt.result.usedBytes)
            userInfo.totalBytes = fileDetailsHelper.getHRSize(receipt.result.totalBytes)
        }
    }

    // Open files conditionally after download
    Connections {
        target: accountWorkers.transferCommandQueue
        onCommandFinished: {
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

            console.log("transfer command finished")
            const isFileDownload = (receipt.info.property("type") === "fileDownload")
            if (!isFileDownload)
                return;

            const fileOpenRequested = receipt.info.property("fileOpen");
            const fileDestination = "file://" + receipt.info.property("localPath");
            if (!fileOpenRequested)
                return

            openFileDestination(fileDestination)
        }
    }

    QtObject {
        id: userInfo
        property bool enabled: false
        property string displayName : ""
        property string email : ""
        property string freeBytes : ""
        property string usedBytes : ""
        property string totalBytes : ""
    }

    Dialog {
        id: fileExistsDialog
        standardButtons: Dialog.Ok | Dialog.Cancel
        parent: pageFlowItemRoot.parent
        width: parent.width / 2
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        property string fileName : ""
        property string path : ""
        property string mimeType : ""
        property bool openFile : false
        property var lastModified : null
        property var transferCommandQueue : null

        Text {
            text: qsTr("Would you like to remove the " +
                       "existing file '%1' before " +
                       "starting the download?").arg(fileExistsDialog.fileName)
            width: parent.width
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        onAccepted: {
            console.debug("Yes")
            FilePathUtil.removeFile(path)
            transferCommandQueue.fileDownloadRequest(path, mimeType, openFile, lastModified)
            transferCommandQueue.run()
        }
        /*onDiscard: {
            console.debug("Discard")
        }*/
    }

    function openFileDestination(fileDestination) {
        if (osIsUbuntuTouch) {
            var contentHubOpener =
            contentHubOpenerComponent.createObject(dialogLayer,
                                                   {
                                                       fileUri : fileDestination
                                                   })
            if (!contentHubOpener) {
                console.error(contentHubOpenerComponent.errorString())
                return;
            }

            dialogLayer.widthChanged.connect(function(){
                contentHubOpener.width = dialogLayer.width
            });
            dialogLayer.heightChanged.connect(function(){
                contentHubOpener.height = dialogLayer.height
            });
            contentHubOpener.width = dialogLayer.width
            contentHubOpener.height = dialogLayer.height
            contentHubOpener.open()
        } else {
            Qt.openUrlExternally(fileDestination)
        }
    }

    function startDownload(path, mimeType, open, overwriteExistingFile, lastModified, transferCommandQueue) {
        const destinationDir = FilePathUtil.destination(accountWorkers.account)
        const fileName = path.substring(path.lastIndexOf("/") + 1)
        const localFilePath = destinationDir + "/" + fileName
        console.log("remote path: " + path)
        console.log("fileExists: " + localFilePath + "?")

        const exists = FilePathUtil.fileExists(localFilePath);

        if (!overwriteExistingFile && exists) {
            fileExistsDialog.fileName = fileName
            fileExistsDialog.path = path
            fileExistsDialog.mimeType = mimeType
            fileExistsDialog.openFile = open
            fileExistsDialog.lastModified = lastModified
            fileExistsDialog.transferCommandQueue = transferCommandQueue
            fileExistsDialog.open()
            return
        }

        transferCommandQueue.fileDownloadRequest(path, mimeType, open, lastModified)
        transferCommandQueue.run()
    }

    Connections {
        target: pageFlowItemRoot.accountWorkers.browserCommandQueue
        onCommandFinished: {
            console.log("onCommandFinished")

            if (!pageFlowItemRoot.enabled) {
                console.debug("disabled")
                return
            }

            const isDavListCommand = (receipt.info.property("type") === "davList")
            const isDavMoveCommand = (receipt.info.property("type") === "davMove")
            const isDavCopyCommand = (receipt.info.property("type") === "davCopy")
            const isDavRmCommand = (receipt.info.property("type") === "davRemove")

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

                const remotePath = receipt.info.property("remotePath")
                const dirContent = receipt.result.dirContent;
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

                // TODO
                if (!receipt.result.success) {
                    notificationRequest(
                                qsTr("Error occured"),
                                qsTr("Please check your credentials or try again later."))
                    return
                }

                // Complete pending PageStack animation
                /* if (pageStack.busy)
                    pageStack.completeAnimation()*/

                var nextDirectory = browserComponent.createObject(pageStack,
                                                                  {
                                                                      remotePath : remotePath,
                                                                      accountWorkers: accountWorkers,
                                                                      pageFlow: pageFlowItemRoot,
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
