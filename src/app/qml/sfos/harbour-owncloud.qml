import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import Nemo.Notifications 1.0
import "qrc:/qml/sfos/navigation"
import "qrc:/qml/sfos/pages/login"

ApplicationWindow
{
    id: applicationWindow
    initialPage: accountSelection
    cover: coverPage

    AccountWorkerGenerator {
        id: accountWorkerGenerator
        database: accountsDb
    }

    OcsUserInfo {
        id: ocsUserInfo
        //commandQueue: null // Set when switching accounts
    }
    DaemonControl {
        id: daemonCtrl
    }
    Notification {
        id: notifier
    }
    QmlMap {
        id: directoryContents
        onInserted: {
            console.log("Inserted contents for path " + key)
        }
    }
    property alias dirContents : directoryContents
    property alias directoryContents : directoryContents

    AccountSelection {
        id: accountSelection
        accountGenerator: accountWorkerGenerator
        browserPage: browserComponent
        dirContents: directoryContents
    }

    readonly property bool isTransfering :
        (daemonCtrl.uploading)

    // Main application cover
    readonly property Component coverPage :
        Qt.createComponent("qrc:/qml/sfos/cover/CoverPage.qml",
                           Component.PreferSynchronous)

    // Application pages
    readonly property Component remoteDirDialogComponent :
        Qt.createComponent("qrc:/qml/sfos/pages/browser/RemoteDirSelectDialog.qml",
                           Component.PreferSynchronous);
    readonly property Component browserComponent :
        Qt.createComponent("qrc:/qml/sfos/pages/browser/FileBrowser.qml",
                           Component.PreferSynchronous);
    readonly property Component fileDetailsComponent :
        Qt.createComponent("qrc:/qml/sfos/pages/browser/FileDetails.qml",
                           Component.PreferSynchronous);
    readonly property Component selectionDialogComponent :
        Qt.createComponent("qrc:/sailfish-ui-set/FileSelectionDialog.qml",
                           Component.PreferSynchronous);
    readonly property Component textEntryDialogComponent :
        Qt.createComponent("qrc:/sailfish-ui-set/TextEntryDialog.qml",
                           Component.PreferSynchronous);
    readonly property Component authenticationEntranceComponent :
        Qt.createComponent("qrc:/qml/sfos/pages/login/Entrance.qml",
                           Component.PreferSynchronous);
    readonly property Component flowAuthenticationComponent :
        Qt.createComponent("qrc:/qml/sfos/pages/login/FlowAuthentication.qml",
                           Component.PreferSynchronous);
    readonly property Component basicAuthenticationComponent :
        Qt.createComponent("qrc:/qml/sfos/pages/login/BasicAuthentication.qml",
                           Component.PreferSynchronous);
    readonly property Component sslErrorDialogComponent :
        Qt.createComponent("qrc:/qml/sfos/pages/login/SSLErrorDialog.qml",
                           Component.PreferSynchronous)
    readonly property Component settingsPageComponent :
        Qt.createComponent("qrc:/qml/sfos/pages/SettingsPage.qml",
                           Component.PreferSynchronous)
    readonly property Component transferPageComponent :
        Qt.createComponent("qrc:/qml/sfos/pages/TransferPage.qml",
                           Component.PreferSynchronous)
    readonly property Component aboutPageComponent :
        Qt.createComponent("qrc:/qml/sfos/pages/About.qml",
                           Component.PreferSynchronous)


    function refreshUserInfo() {
        // Skip in case there's already a userInfo command in the queue
        for (var i = 0; i < accountInfoCommandQueue.queue.length; i++) {
            var commandInfo = accountInfoCommandQueue.queue[i].info;
            if (commandInfo.type === "userInfo")
                return;
        }

        /*if (transferQueue.queue.length < 1) {
            ocsCommandQueue.userInfoRequest();
            ocsCommandQueue.run()
        }*/
    }

    function notify(summary, body) {
        notifier.previewSummary = summary
        notifier.previewBody = body
        notifier.isTransient = false
        notifier.publish();
    }

    function notifyTransient(summary) {
        notifier.previewBody = ""
        notifier.previewSummary = summary
        notifier.isTransient = true
        notifier.publish();
    }
}


