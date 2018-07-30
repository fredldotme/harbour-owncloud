import QtQuick 2.0
import harbour.owncloud 1.0

CommandPageFlow {
    Connections {
        target: commandQueue
        onCommandFinished: {
            console.log("onCommandFinished")
            var isDavListCommand = (receipt.info.property("type") === "davList")
            var isDavMoveCommand = (receipt.info.property("type") === "davMove")
            var isDavCopyCommand = (receipt.info.property("type") === "davCopy")
            var isDavRmCommand = (receipt.info.property("type") === "davRemove")

            // Error messages
            if (!receipt.finished) {
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

                var dirContent = receipt.result;
                directoryContents.insert(remotePath, dirContent);

                // Done in case of a refresh
                if (isRefresh)
                    return;

                // Complete pending PageStack animation
                if (pageStack.busy)
                    pageStack.completeAnimation()

                var nextDirectory = browserComponent.createObject(pageStack,
                                                                  { remotePath : remotePath });

                if (!nextDirectory) {
                    console.warn(browserComponent.errorString())
                    return
                }

                // Replace the current top page when requesting a directory
                // lising for the remote root path, ie after initial login.
                if (remotePath === "/") {
                    pageStack.replace(nextDirectory)

                    // Additionally try to fetch the avatar which
                    // will only succeed if the server supports it.
                    avatarFetcher.fetch()
                } else {
                    pageStack.push(nextDirectory)
                }

                return
            }

            // Refresh userInfo after remote removal or copy
            if (isDavRmCommand || isDavCopyCommand) {
                refreshUserInfo()
                return
            }
        }
    }
}
