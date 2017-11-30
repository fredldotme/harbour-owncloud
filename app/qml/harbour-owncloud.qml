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
            pageStack.replace("pages/Login.qml", {}, PageStackAction.Immediate)
            pageStack.push("pages/SSLErrorDialog.qml", {md5Digest : md5Digest, sha1Digest : sha1Digest});
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
            pageStack.push("pages/Login.qml")
        }
    }

    Notification {
        id: notification
    }

    Connections {
        target: transfer
        onUploadComplete: {
            if(settings.notifications)
                notify(qsTr("Upload complete"), qsTr("%1 uploaded successfully").arg(name))
        }
    }

    Connections {
        target: transfer
        onDownloadComplete: {
            if(settings.notifications)
                notify(qsTr("Download complete"), qsTr("%1 downloaded successfully").arg(name))
        }
    }

    Connections {
        target: transfer
        onUploadFailed: {
            if(settings.notifications)
                notify(qsTr("Upload failed!"), qsTr("%1 couldn't be uploaded").arg(name))
        }
    }

    Connections {
        target: transfer
        onDownloadFailed: {
            if(settings.notifications)
                notify(qsTr("Download failed!"), qsTr("%1 couldn't be downloaded").arg(name))
        }
    }

    Connections {
        target: transfer
        onLocalMtimeFailed: {
            if(settings.notifications)
                notify("Modification time failed!", "Setting mtime failed, errno " + status)
        }
    }

    Connections {
        target: transfer
        onRemoteMtimeFailed: {
            if(settings.notifications)
                notify("Modification time failed!", "Setting mtime failed, status " + status)
        }
    }

    id: applicationWindow
    initialPage: Component { Login { id: loginPage } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
}


