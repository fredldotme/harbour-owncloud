import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import SailfishUiSet 1.0
import Nemo.Notifications 1.0
import "qrc:/qml/sfos/pages/browser/controls"
import "qrc:/qml/sfos/navigation"
import "qrc:/sailfish-ui-set"

Page {
    id: pageRoot
    //anchors.fill: parent

    property Component contextMenuComponent :
        Qt.createComponent(
            "qrc:/qml/sfos/pages/browser/controls/FileOperationsContextMenu.qml",
            Component.PreferSynchronous);

    property string remotePath : "/"
    property string pageHeaderText : "/"

    property var accountWorkers : null
    property BrowserCommandPageFlow pageFlow : null

    property var browserCommandQueue : accountWorkers.browserCommandQueue
    property var transferCommandQueue : accountWorkers.transferCommandQueue

    // Keep track of directory listing requests
    property var __listCommand : null

    // Keep track of active dialog.
    // There can only be one of them active at a time due to their modal nature.
    property var dialogObj : null

    // Disable pull down menu items while
    // reloading content of current directory.
    readonly property bool __enableMenuItems :
        (listView.model !== undefined &&
         __listCommand === null);

    function __dialogCleanup() {
        dialogObj = null
    }

    function refreshListView(refresh) {
        __listCommand = accountWorkers.browserCommandQueue.directoryListingRequest(remotePath, refresh)
    }

    function changeDirectory(newPath) {
        pageFlow.targetRemotePath = newPath
        __listCommand = accountWorkers.browserCommandQueue.directoryListingRequest(newPath, false)
    }

    signal transientNotification(string summary)
    signal notification(string summary, string body)

    FileDetailsHelper { id: fileDetailsHelper }

    onStatusChanged: {
        if (status === PageStatus.Inactive) {
            if (_navigation !== undefined && _navigation === PageNavigation.Back) {
                if (__listCommand !== null)
                    __listCommand.abort(true)
                directoryContents.remove(remotePath)
                pageRoot.destroy()
            }
        }
    }

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
        target: pageRoot.accountWorkers.transferCommandQueue
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
            console.log("key:" + key + " remotePath:" + remotePath)
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

        ContextMenu {
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
                        value: ocsUserInfo.displayName
                        visible: value.length > 0
                    }
                    DetailItem {
                        width: parent.width
                        label: qsTr("Mail:")
                        value: ocsUserInfo.emailAddress
                        visible: value.length > 0
                    }
                    DetailItem {
                        width: parent.width
                        label: qsTr("Usage:")
                        value: ocsUserInfo.hrUsedBytes
                        visible: value.length > 0
                    }
                    DetailItem {
                        width: parent.width
                        label: qsTr("Free:")
                        value: ocsUserInfo.hrFreeBytes
                        visible: value.length > 0
                    }
                    DetailItem {
                        width: parent.width
                        label: qsTr("Total:")
                        value: ocsUserInfo.hrTotalBytes
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
                        anchors.margins: Theme.paddingLarge
                        state: userInformation.active ? "visible" : "invisible"
                        enabled: false
                    }
                }
            }
        }
    }

    SilicaListView {
        id: listView
        anchors.top: userInformationAnchor.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        model: directoryContents.value(remotePath)

        PullDownMenu {
            MenuItem {
                text: qsTr("Refresh")
                enabled: __enableMenuItems
                onClicked: {
                    refreshListView(true)
                }
            }

            MenuItem {
                text:qsTr("Create directory")
                enabled: __enableMenuItems
                onClicked: {
                    dialogObj = textEntryDialogComponent.createObject(pageRoot);
                    dialogObj.placeholderText = qsTr("Directory name")
                    dialogObj.labelText = dialogObj.placeholderText
                    dialogObj.accepted.connect(function () {
                        var newPath = remotePath + dialogObj.text
                        browserCommandQueue.makeDirectoryRequest(newPath)
                        refreshListView(true)
                        __dialogCleanup()
                    })
                    dialogObj.rejected.connect(__dialogCleanup)
                    pageStack.push(dialogObj)
                }
            }

            MenuItem {
                function enqueueSelectedFiles() {
                    var selectedFiles = dialogObj.filesToSelect
                    for (var i = 0; i < selectedFiles.length; i++) {
                        var canonicalRemotePath = FilePathUtil.getCanonicalPath(remotePath);
                        pageRoot.accountWorkers.transferCommandQueue.fileUploadRequest(selectedFiles[i].path,
                                                                                       canonicalRemotePath,
                                                                                       selectedFiles[i].lastModified);
                    }
                    pageRoot.accountWorkers.transferCommandQueue.run()
                    __dialogCleanup()
                }

                text: qsTr("Upload")
                enabled: __enableMenuItems
                onClicked: {
                    dialogObj = selectionDialogComponent.createObject(pageRoot,
                                                                      {maximumSelections:Number.MAX_VALUE});
                    dialogObj.acceptText = qsTr("Upload")
                    dialogObj.errorOccured.connect(function(errorStr) {
                        transientNotification(errorStr)
                    });
                    dialogObj.accepted.connect(enqueueSelectedFiles)
                    dialogObj.rejected.connect(__dialogCleanup)
                    pageStack.push(dialogObj)
                }
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("File transfers")
                onClicked: {
                    var transferPage =
                            transferPageComponent.createObject(pageRoot,
                                                               {
                                                                   accountWorkers : accountWorkers
                                                               });
                    pageStack.push(transferPage)
                }
            }
            MenuItem {
                text: qsTr("About")
                onClicked: {
                    pageStack.push(aboutPageComponent)
                }
            }
        }

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

        header: PageHeader {
            id: pageHeader
            title: pageHeaderText
            width: listView.width

            CircularImageButton {
                property bool __showAvatar :
                    (accountWorkers.account.providerType === AccountBase.Nextcloud)
                source: __showAvatar ? accountWorkers.avatarFetcher.source : ""
                enabled: __showAvatar
                visible: ocsUserInfo.userInfoEnabled
                highlightColor:
                    Qt.rgba(Theme.highlightColor.r,
                            Theme.highlightColor.g,
                            Theme.highlightColor.b,
                            0.5)

                readonly property bool visibility :
                    (!userInformation.active &&
                     (pageRoot.status === PageStatus.Active ||
                      pageRoot.status === PageStatus.Activating))

                readonly property int __leftMargin :
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

        delegate: ListItem {
            id: delegate
            enabled: (__listCommand === null)
            contentHeight: mainEntryItem.height

            property var davInfo : listView.model[index]

            Item {
                id: mainEntryItem
                width: parent.width
                height: itemLabel.height + entryDetails.height + Theme.paddingSmall

                Image {
                    id: icon
                    source: davInfo.isDirectory ?
                                "image://theme/icon-m-folder" :
                                fileDetailsHelper.getIconFromMime(davInfo.mimeType)
                    enabled: parent.enabled
                    fillMode: Image.PreserveAspectFit
                    height: Math.min(Theme.iconSizeMedium, parent.height)
                    width: height
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                        leftMargin: Theme.paddingSmall
                    }
                }
                Item {
                    anchors {
                        left: icon.right
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                        leftMargin: Theme.paddingSmall
                    }
                    height: itemLabel.height + entryDetails.height

                    Label {
                        id: itemLabel
                        anchors {
                            left: parent.left
                            top: parent.top
                        }
                        enabled: parent.enabled
                        text: davInfo.name
                        color: delegate.highlighted ?
                                   Theme.highlightColor :
                                   Theme.primaryColor
                        truncationMode: TruncationMode.Fade
                        font.pixelSize: Theme.fontSizeSmall
                    }

                    Label {
                        anchors {
                            left: parent.left
                            top: itemLabel.bottom
                        }
                        id: entryDetails
                        enabled: parent.enabled
                        text: fileDetailsHelper.getHRSize(davInfo.size)
                              + (!davInfo.isDirectory ?
                                     (", " +
                                      Qt.formatDateTime(davInfo.lastModified, Qt.SystemLocaleShortDate)) : "")
                        color: delegate.highlighted ?
                                   Theme.highlightColor :
                                   Theme.primaryColor
                        truncationMode: TruncationMode.Fade
                        font.pixelSize: Theme.fontSizeExtraSmall
                    }
                }
            }

            function showDetails() {
                var fileDetails = fileDetailsComponent.createObject(pageRoot, {
                                                                        entry: davInfo,
                                                                        accountWorkers: accountWorkers
                                                                    });
                if (!fileDetails) {
                    console.warn(fileDetailsComponent.errorString())
                    return;
                }

                pageStack.push(fileDetails);
            }

            onClicked: {
                if(davInfo.isDirectory) {
                    var nextPath = remotePath + davInfo.name + "/";
                    changeDirectory(nextPath)
                } else {
                    showDetails();
                }
            }

            // Delete menu resources after completion,
            // which MUST be indicated by the context menu component
            onPressAndHold: {
                console.log(JSON.stringify(davInfo))
                if (!menu) {
                    menu = contextMenuComponent.createObject(listView, {
                                                                 selectedEntry : davInfo,
                                                                 selectedItem : delegate,
                                                                 dialogObj: dialogObj,
                                                                 remoteDirDialogComponent : remoteDirDialogComponent,
                                                                 textEntryDialogComponent : textEntryDialogComponent,
                                                                 fileDetailsComponent : fileDetailsComponent,
                                                                 transferCommandQueue : pageRoot.accountWorkers.transferCommandQueue,
                                                                 browserCommandQueue : pageRoot.accountWorkers.browserCommandQueue
                                                             })
                    menu.detailsRequested.connect(showDetails)
                    menu.requestListReload.connect(refreshListView)
                    menu.contextMenuDone.connect(function() {
                        menu.destroy()
                        menu = null
                    });
                }

                openMenu()
            }
        }
        VerticalScrollDecorator {}

        AbortableBusyIndicator {
            running: (__listCommand !== null)
            enabled: running
            size: BusyIndicatorSize.Large
            buttonVisibiltyDelay: 5000
            anchors.centerIn: parent
            onAbort: {
                if (!__listCommand)
                    return
                __listCommand.abort(true)
            }
        }

        ViewPlaceholder {
            text: qsTr("Folder is empty")
            enabled: (__listCommand === null &&
                      listView.model.length < 1)
        }
    }
}
