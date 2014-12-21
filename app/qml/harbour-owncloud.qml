import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import "pages"

ApplicationWindow
{
    property bool loginInProgress : false;

    function getIconFromMime(mimeType) {
        if(mimeType.indexOf("image") === 0) {
            return "image://theme/icon-l-image";
        } else if(mimeType.indexOf("audio") === 0) {
            return "image://theme/icon-l-music";
        } else if(mimeType.indexOf("video") === 0) {
            return "image://theme/icon-l-video";
        } else {
            return "image://theme/icon-l-document";
        }
    }

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
            notify("Login failed", "Please check your host address and credentials")

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
                notify("Upload complete", name + " uploaded successfully")
        }
    }

    Connections {
        target: transfer
        onDownloadComplete: {
            if(settings.notifications)
                notify("Download complete", name + " downloaded successfully")
        }
    }

    Connections {
        target: transfer
        onUploadFailed: {
            if(settings.notifications)
                notify("Upload failed!", name + " couldn't be uploaded")
        }
    }

    Connections {
        target: transfer
        onDownloadFailed: {
            if(settings.notifications)
                notify("Download failed!", name + " couldn't be downloaded")
        }
    }

    id: applicationWindow
    initialPage: Component { Login { id: loginPage } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
}


