import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import harbour.owncloud 1.0
import QmlUiSet 1.0
import "qrc:/qml/qqc"
import "qrc:/qml/qqc/controls"
import "qrc:/qml/qqc/dialogs"
import "qrc:/qml-ui-set"

Page {
    id: pageRoot
    objectName: "FileBrowser"
    title: directoryName
    focus: true

    property string remotePath : "/"
    property string directoryName : {
        var crumbs = remotePath.split("/").filter(function (element) {
            return element !== null && element !== "";
        });

        if (crumbs.length < 1)
            return "/"

        return crumbs[(crumbs.length - 1) % crumbs.length]
    }
    property string pageHeaderText : "/"
    property var accountWorkers : null

    property StackView detailsStack : null

    property BrowserCommandPageFlow pageFlow : null
    property QmlMap directoryContents : rootWindow.dirContents

    property var browserCommandQueue : accountWorkers.browserCommandQueue
    property var transferCommandQueue : accountWorkers.transferCommandQueue
    property var userInfoCommandQueue : accountWorkers.accountInfoCommandQueue

    property alias fileUploadDialog : openFileDialog
    property alias avatarMenu : avatarMenu

    property var copyEntryDialog : pageFlow.copyEntryDialog
    property var moveEntryDialog : pageFlow.moveEntryDialog
    property var renameDialog : pageFlow.renameDialog
    property var dirCreationDialog : pageFlow.dirCreationDialog

    // Keep track of directory listing requests
    property var __listCommand : null
    readonly property bool isBusy : __listCommand !== null

    // Disable pull down menu items while
    // reloading content of current directory.
    readonly property bool __enableMenuItems :
        (listView.model !== undefined &&
         __listCommand === null);

    function refreshListView(refresh) {
        __listCommand = browserCommandQueue.directoryListingRequest(remotePath, refresh)
    }

    function changeDirectory(newPath) {
        pageFlow.targetRemotePath = newPath
        __listCommand = browserCommandQueue.directoryListingRequest(newPath, false)
    }

    function fileSelectIntent() {

    }

    function openDetails(davEntryInfo) {
        var fileDetails =
                fileDetailsComponent.createObject(rootWindow.detailsStack,
                                                  {
                                                      entry: davEntryInfo,
                                                      accountWorkers: accountWorkers
                                                  });
        if (!fileDetails) {
            console.warn(fileDetailsComponent.errorString())
            return;
        }

        if (rootWindow.detailsStack!== rootStack)
            rootWindow.detailsStack.pop()

        rootWindow.detailsStack.push(fileDetails);
    }

    function newDirectoryDialogOpen() {
        pageFlow.dirCreationDialog.open()
    }

    signal transientNotification(string summary)
    signal notification(string summary, string body)

    FileDialog {
        id: openFileDialog
        selectMultiple: true

        onAccepted: {
            for (var i = 0; i < fileUrls.length; i++) {
                console.log("Enqueueing upload " + fileUrls[i] + " to " + pageRoot.remotePath)
                transferCommandQueue.fileUploadRequest(fileUrls[i],
                                                       pageRoot.remotePath)
            }
            transferCommandQueue.run()
        }
    }

    Connections {
        target: dirCreationDialog
        onAccepted: {
            browserCommandQueue.makeDirectoryRequest(
                        pageRoot.remotePath + dirCreationDialog.text)
            refreshListView(true)
            dirCreationDialog.text = ""
        }
    }

    Connections {
        target: renameDialog
        onAccepted: {
            browserCommandQueue.moveRequest(
                        rightClickMenu.selectedDavInfo.path,
                        pageRoot.remotePath + renameDialog.text)
            refreshListView(true)
            renameDialog.text = ""
        }
    }

    Connections {
        target: moveEntryDialog

        onAccepted: {
            console.debug("accepted")
            browserCommandQueue.moveRequest(rightClickMenu.selectedDavInfo.path,
                                            moveEntryDialog.remotePath + rightClickMenu.selectedDavInfo.name)
            refreshListView(true)
        }
    }

    Connections {
        target: copyEntryDialog

        onAccepted: {
            console.debug("accepted")
            browserCommandQueue.copyRequest(rightClickMenu.selectedDavInfo.path,
                                            copyEntryDialog.remotePath + rightClickMenu.selectedDavInfo.name)
            refreshListView(true)
        }
    }

    FileDetailsHelper { id: fileDetailsHelper }

    /*onStatusChanged: {
        if (status === PageStatus.Inactive) {
            if (_navigation !== undefined && _navigation === PageNavigation.Back) {
                if (__listCommand !== null)
                    __listCommand.abort(true)
                directoryContents.remove(remotePath)
                pageRoot.destroy()
            }
        }
    }*/

    onRemotePathChanged: {
        if (remotePath === "/") {
            pageHeaderText = "/";
        } else {
            var dirs = remotePath.split("/")
            if(dirs.length > 1) {
                pageHeaderText = dirs[dirs.length - 2]
            } else {
                pageHeaderText = ""
            }
        }
    }

    Menu {
        id: avatarMenu
        DetailItem {
            width: parent.width
            label: qsTr("User:")
            value: pageFlow.userInfo.displayName
        }
        /*DetailItem {
            width: parent.width
            label: qsTr("Mail:")
            value: pageFlow.userInfo.email
        }*/
        DetailItem {
            width: parent.width
            label: qsTr("Usage:")
            value: pageFlow.userInfo.usedBytes
        }
        DetailItem {
            width: parent.width
            label: qsTr("Free:")
            value: pageFlow.userInfo.totalBytes
        }
        DetailItem {
            width: parent.width
            label: qsTr("Total:")
            value: pageFlow.userInfo.totalBytes
        }
        /*MouseArea {
            width: parent.width - paddingSmall*2
            height: settingsButton.height
            anchors.horizontalCenter: parent.horizontalCenter
            Label {
                id: settingsButton
                text: qsTr("Settings")
                anchors.horizontalCenter: parent.horizontalCenter
                font.underline: true
                color: "blue"
            }
            onClicked: {
                Qt.openUrlExternally(accountWorkers.providerSettingsUrl)
            }
        }*/
    }

    Connections {
        target: browserCommandQueue
        onCommandStarted: {
            // Get the handle to davList commands when remotePaths match
            var isDavListCommand = (command.info.property("type") === "davList")
            if (!isDavListCommand)
                return;

            // Don't replace existing handle
            if (!__listCommand && command.info.property("remotePath") === remotePath) {
                __listCommand = command;
            }
        }

        onCommandFinished: {
            // Invalidate __listCommand after completion
            if (receipt.info.property("type") === "davList") {
                __listCommand = null
                return;
            }

            // Refresh the listView after a non-davList command
            // related to this remote directory succeeded
            if (receipt.info.property("remotePath") === pageRoot.remotePath) {
                refreshListView(true)
                return;
            }
        }
    }

    Connections {
        target: transferCommandQueue
        onCommandFinished: {
            // Refresh the listView after an upload to this remote directory succeeded
            if (receipt.info.property("remotePath") === pageRoot.remotePath) {
                refreshListView(true)
                return;
            }
        }
    }

    Connections {
        target: directoryContents
        onInserted: {
            // console.log("key:" + key + " remotePath:" + remotePath)
            if (key !== remotePath)
                return;

            // TODO: apply delta between directoryContents and localListModel
            // TODO: animation when inserting/deleting entries
            listView.model = directoryContents.value(key)
        }
    }

    Menu {
        id: rightClickMenu
        property var selectedDavInfo : null

        MenuItem {
            enabled: rightClickMenu.selectedDavInfo &&
                     (!rightClickMenu.selectedDavInfo.isDirectory)
            text: qsTr("Open")
            font.pixelSize: fontSizeSmall
            onClicked: {
                startDownload(rightClickMenu.selectedDavInfo.path,
                              rightClickMenu.selectedDavInfo.mimeType,
                              true,
                              false,
                              rightClickMenu.selectedDavInfo.lastModified,
                              transferCommandQueue)
            }
        }
        MenuItem {
            enabled: rightClickMenu.selectedDavInfo &&
                     (!rightClickMenu.selectedDavInfo.isDirectory)
            text: qsTr("Download")
            font.pixelSize: fontSizeSmall
            onClicked: {
                startDownload(rightClickMenu.selectedDavInfo.path,
                              rightClickMenu.selectedDavInfo.mimeType,
                              false,
                              false,
                              rightClickMenu.selectedDavInfo.lastModified,
                              transferCommandQueue)
            }
        }

        MenuSeparator {}

        MenuItem {
            enabled: true
            text: qsTr("Rename")
            font.pixelSize: fontSizeSmall
            onClicked: {
                renameDialog.text = rightClickMenu.selectedDavInfo.name
                renameDialog.open()
            }
        }

        MenuItem {
            enabled: true
            text: qsTr("Copy")
            font.pixelSize: fontSizeSmall
            onClicked: {
                copyEntryDialog.open()
            }
        }
        MenuItem {
            enabled: true
            text: qsTr("Move")
            font.pixelSize: fontSizeSmall
            onClicked: {
                moveEntryDialog.open()
            }
        }
        MenuItem {
            enabled: true
            text: qsTr("Delete")
            font.pixelSize: fontSizeSmall
            onClicked: {
                browserCommandQueue.removeRequest(rightClickMenu.selectedDavInfo.path)
                refreshListView(true)
            }
        }

        MenuSeparator {}

        MenuItem {
            enabled: true
            text: qsTr("Details")
            font.pixelSize: fontSizeSmall
            onClicked: openDetails(rightClickMenu.selectedDavInfo, accountWorkers);
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        clip: true
        model: directoryContents.value(remotePath)
        ScrollBar.vertical: ScrollBar {}

        add: Transition {
            NumberAnimation {
                properties: "height"
                from: 0
                to: delegate.height
                duration: 200
            }
        }
        remove: Transition {
            NumberAnimation {
                properties: "height"
                from: delegate.height
                to: 0
                duration: 200
            }
        }

        delegate: MouseArea {
            id: delegate
            width: listView.width
            height: childrenRect.height
            enabled: (__listCommand === null)
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressAndHold: {
                entryContextMenu(delegate, mouseX, mouseY)
            }
            onClicked: {
                entryClickHandler(mouse)
            }

            property var davInfo : listView.model[index]

            function entryContextMenu(newParent, mouseX, mouseY) {
                rightClickMenu.selectedDavInfo = davInfo
                rightClickMenu.parent = newParent
                rightClickMenu.x = mouseX
                rightClickMenu.y = mouseY
                rightClickMenu.open()
            }

            function entryClickHandler(mouse) {
                if (mouse.button === Qt.RightButton) {
                    entryContextMenu(delegate, mouseX, mouseY)
                    return;
                }

                if (davInfo.isDirectory) {
                    var nextPath = remotePath + davInfo.name + "/";
                    changeDirectory(nextPath)
                } else {
                    openDetails(davInfo);
                }
            }

            Column {
                enabled: parent.enabled
                width: listView.width
                height: 48
                GCButton {
                    id: icon
                    source: davInfo.isDirectory
                               ? "qrc:/icons/theme/places/64/folder.svg"
                               : fileDetailsHelper.getIconFromMime(davInfo.mimeType)
                    text: davInfo.name
                    detailText: fileDetailsHelper.getHRSize(davInfo.size)
                                + (!davInfo.isDirectory ?
                                       (", " +
                                        Qt.formatDateTime(davInfo.lastModified, Qt.SystemLocaleShortDate)) : "")
                    height: parent.height
                    font.pixelSize: fontSizeSmall
                    MouseArea {
                        anchors.fill: parent
                        onClicked: entryClickHandler(mouse)
                    }
                }
            }
        }

        AbortableBusyIndicator {
            running: (__listCommand !== null)
            enabled: running
            buttonVisibiltyDelay: 5000
            anchors.centerIn: parent
            onAbort: {
                if (!__listCommand)
                    return
                __listCommand.abort(true)
            }
        }

        Label {
            text: qsTr("Folder is empty")
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            visible: (__listCommand === null &&
                      listView.model.length < 1)
            font.pixelSize: fontSizeLarge
            anchors.centerIn: parent
        }
    }
}
