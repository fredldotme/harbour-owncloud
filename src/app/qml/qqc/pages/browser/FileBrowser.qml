import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import harbour.owncloud 1.0
import QmlUiSet 1.0
import "qrc:/qml-ui-set"
import "qrc:/qml/qqc"
import "qrc:/qml/qqc/controls"
import "qrc:/qml/qqc/dialogs"

Page {
    id: pageRoot
    objectName: "FileBrowser"
    title: qsTr("Files")

    property string remotePath : "/"
    property string pageHeaderText : "/"
    property var accountWorkers : null

    property StackView detailsStack : null

    property BrowserCommandPageFlow pageFlow : null
    property QmlMap directoryContents : rootWindow.dirContents

    property var browserCommandQueue : accountWorkers.browserCommandQueue
    property var transferCommandQueue : accountWorkers.transferCommandQueue

    property alias fileUploadDialog : openFileDialog
    property alias newDirectoryDialog : dirCreationDialog

    property var copyEntryDialog : null
    property var moveEntryDialog : null

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

    TextEntryDialog {
        id: dirCreationDialog
        title: qsTr("Enter directory name:")

        onAccepted: {
            browserCommandQueue.makeDirectoryRequest(
                        pageRoot.remotePath + dirCreationDialog.text)
            refreshListView(true)
            dirCreationDialog.text = ""
        }
    }

    TextEntryDialog {
        id: renameDialog
        title: qsTr("Enter new name:")

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

    Connections {
        target: pageRoot.accountWorkers.browserCommandQueue
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

    Item {
        id: userInformationAnchor
        width: parent.width

        // Rely on height of the userInformation column
        // for the CircularImageButton to adapt automatically
        height: userInformation.height
        //onHeightChanged: console.log(height)
        property bool active: true

        Menu {
            id: userInformation
            width: userInformationAnchor.width

            RowLayout {
                width: parent.width

                Column {
                    id: detailsColumn
                    width: (parent.width / 3) * 2
                    DetailItem {
                        width: parent.width
                        label: qsTr("User:")
                        //value: ocsUserInfo.displayName
                        visible: value.length > 0
                    }
                    DetailItem {
                        width: parent.width
                        label: qsTr("Mail:")
                        //value: ocsUserInfo.emailAddress
                        visible: value.length > 0
                    }
                    DetailItem {
                        width: parent.width
                        label: qsTr("Usage:")
                        //value: ocsUserInfo.hrUsedBytes
                        visible: value.length > 0
                    }
                    DetailItem {
                        width: parent.width
                        label: qsTr("Total:")
                        //value: ocsUserInfo.hrTotalBytes
                        visible: value.length > 0
                    }
                }

                Item {
                    width: (parent.width / 3)
                    height: width
                    CircularImageButton {
                        source: accountWorkers.avatarFetcher.source
                        Layout.fillWidth: true
                        anchors.fill: parent
                        anchors.margins: 24
                        state: userInformation.active ? "visible" : "invisible"
                        enabled: false
                    }
                }
            }
        }
    }

    Menu {
        id: rightClickMenu
        property var selectedDavInfo : null

        MenuItem {
            enabled: true
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
            enabled: true
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
/*
        header: ToolBar {
            id: pageHeader
            width: listView.width

            CircularImageButton {
                property bool __showAvatar :
                    (accountWorkers.account.providerType === NextcloudSettings.Nextcloud)
                source: __showAvatar ? accountWorkers.avatarFetcher.source : ""
                enabled: __showAvatar
                visible: accountWorkers.accountInfoCommandQueue.userInfoEnabled
                highlightColor:
                    Qt.rgba(200,
                            0,
                            0,
                            0.5)

                readonly property bool visibility :
                    (!userInformation.active)

                // Keep smaller padding on the root
                readonly property int __leftMargin :
                    (remotePath === "/") ?
                        (Theme.horizontalPageMargin) :
                        (Theme.horizontalPageMargin*2.5)

                // onVisibilityChanged: console.log("visibility: " + visibility)
                state: visibility ? "visible" : "invisible"

                anchors {
                    top: parent.top
                    topMargin: (Theme.paddingMedium * 1.2)
                    left: parent.left
                    leftMargin: __leftMargin
                    bottom: parent.bottom
                    bottomMargin: (Theme.paddingMedium * 1.2)
                }

                width: height
                onClicked: { userInformation.open(userInformationAnchor) }
            }
        }
*/
        delegate: Row {
            id: delegate
            width: parent.width
            enabled: (__listCommand === null)

            property var davInfo : listView.model[index]

            Button {
                id: icon
                icon.color: "transparent"
                icon.name: davInfo.isDirectory ? "folder" :
                            fileDetailsHelper.getIconFromMime(davInfo.mimeType)
                enabled: parent.enabled
                text: davInfo.name
                font.pixelSize: fontSizeSmall
                background: Rectangle { color: "transparent" }

                MouseArea {
                    id: labelMouseArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    function entryContextMenu() {
                        rightClickMenu.selectedDavInfo = davInfo
                        rightClickMenu.popup(delegate,
                                            labelMouseArea.mouseX,
                                            labelMouseArea.Y)
                    }

                    onClicked: {
                        if (mouse.button === Qt.RightButton) {
                            entryContextMenu()
                            return;
                        }

                        if (davInfo.isDirectory) {
                            var nextPath = remotePath + davInfo.name + "/";
                            changeDirectory(nextPath)
                        } else {
                            openDetails(davInfo);
                        }
                    }
                }
            }

            // Delete menu resources after completion,
            // which MUST be indicated by the context menu component
            /*onPressAndHold: {
                if (!menu) {
                    menu = contextMenuComponent.createObject(listView, {
                                                                 selectedEntry : davInfo,
                                                                 selectedItem : delegate,
                                                                 dialogObj: dialogObj,
                                                                 remoteDirDialogComponent : remoteDirDialogComponent,
                                                                 textEntryDialogComponent : textEntryDialogComponent,
                                                                 fileDetailsComponent : fileDetailsComponent,
                                                                 transferQueue : transferQueue,
                                                                 browserCommandQueue : browserCommandQueue
                                                             })
                    menu.requestListReload.connect(refreshListView)
                    menu.contextMenuDone.connect(function() {
                        menu.destroy()
                        menu = null
                    });
                }

                openMenu()
            }*/
        }
        //VerticalScrollDecorator {}

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
            visible: (__listCommand === null &&
                      listView.model.length < 1)
            font.pixelSize: fontSizeLarge
            anchors.centerIn: parent
        }
    }
}
