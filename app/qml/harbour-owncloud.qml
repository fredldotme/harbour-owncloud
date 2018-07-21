import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import "pages"

ApplicationWindow
{
    property bool loginInProgress : false;
    property bool loginFailed : false;

    function notify(summary, body) {
        notification.previewSummary = summary
        notification.previewBody = body
        notification.publish();
    }

    Connections {
        target: browser
        onSslCertifcateError: {
            pageStack.completeAnimation();
            pageStack.clear();
            pageStack.replace("qrc:/qml/pages/Login.qml", {}, PageStackAction.Immediate)
            pageStack.push("qrc:/qml/pages/SSLErrorDialog.qml", {md5Digest : md5Digest, sha1Digest : sha1Digest});
        }
    }

    Connections {
        target: browser
        onLoginFailed: {
            loginInProgress = false;
            loginFailed = true;
            notify(qsTr("Login failed"), qsTr("Please check your host address and credentials"))

            pageStack.completeAnimation();
            pageStack.clear();
            pageStack.completeAnimation();
            pageStack.push("qrc:/qml/pages/Login.qml")
        }
    }

    Notification {
        id: notification
    }

    Connections {
        target: transfer.downloadQueue
        onCommandFinished: {
            if (!settings.notifications)
                return;

            var fileName = receipt.info.properties()["fileName"]
            if (receipt.finished)
                notify(qsTr("Download complete"), qsTr("%1 downloaded successfully").arg(fileName))
            else
                notify(qsTr("Download failed!"), qsTr("%1 couldn't be downloaded").arg(fileName))
        }
    }

    Connections {
        target: transfer.uploadQueue
        onCommandFinished: {
            if (!settings.notifications)
                return;

            var fileName = receipt.info.properties()["fileName"]
            if (receipt.finished)
                notify(qsTr("Upload complete"), qsTr("%1 uploaded successfully").arg(fileName))
            else
                notify(qsTr("Upload failed!"), qsTr("%1 couldn't be uploaded").arg(fileName))
        }
    }

    Connections {
        target: transfer.mainQueue
        onCommandFinished: {
            console.log("onCommandFinished")
            if (!receipt.finished)
                return;

            var isDavListCommand = (receipt.info.properties()["type"] === "davList")
            console.log("isDavListCommand " + isDavListCommand)
            if (!isDavListCommand)
                return;

            var remotePath = receipt.info.properties()["remotePath"]
            var dirContent = receipt.result;
            directoryContents.insert(remotePath, dirContent);
        }
    }

    QmlMap {
        id: directoryContents
        onInserted: {
            console.log("INSERTED!")
        }
        onRemoved: {
            console.log("REMOVED!")
        }
    }

    ListModel {
        id: transfers
        dynamicRoles: true
    }
    Connections {
        target: transfer.downloadQueue
        onAdded: {
            transfers.append(entity)
        }
        onRemoved: {
            transfers.remove(entity)
        }
    }
    Connections {
        target: transfer.uploadQueue
        onAdded: {
            transfers.append(entity)
        }
        onRemoved: {
            transfers.remove(entity)
        }
    }

    id: applicationWindow
    initialPage: Component { Login { id: loginPage } }
    cover: Qt.resolvedUrl("qrc:/qml/cover/CoverPage.qml")
}


