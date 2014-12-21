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
            notify("Upload complete", name + " uploaded successfully")
        }
    }

    id: applicationWindow
    initialPage: Component { Login { id: loginPage } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
}


