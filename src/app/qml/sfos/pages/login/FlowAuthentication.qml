import QtQuick 2.0
import Sailfish.Silica 1.0
import QtWebKit 3.0
//import QtWebKit.experimental 1.0
import harbour.owncloud 1.0

Page {
    id: pageRoot
//    anchors.fill: parent

    FlowLoginAuthenticator {
        id: authenticator
        settings: persistentSettings
        onAuthenticationSuccessful: {
            pageStack.completeAnimation()
            pageStack.replace(browserComponent)
        }
        onAuthenticationFailed: {
            pageStack.completeAnimation()
            pageStack.replace(basicAuthenticationComponent)
        }
        onUrlChanged: {
            console.log("FlowLoginAuthenticator url: " + authenticator.url)
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        SilicaWebView {
            id: webView
            anchors.fill: parent
            url: authenticator.url

            /*experimental.userAgent: "Nextcloud/OwnCloud Client for Sailfish"
            experimental.preferences.webGLEnabled: false
            experimental.preferences.localStorageEnabled: false
            experimental.preferences.webAudioEnabled: false
            experimental.preferences.developerExtrasEnabled: false*/

            onNavigationRequested: {
                if (request.url.toString().indexOf("nc://") == 0) {
                    request.action = WebView.IgnoreRequest;
                    authenticator.validateFlowResponse(request.url)
                    return;
                }
                request.action = WebView.AcceptRequest;
                console.log("loadRequest.url: " + request.url)
            }
        }
    }
}
