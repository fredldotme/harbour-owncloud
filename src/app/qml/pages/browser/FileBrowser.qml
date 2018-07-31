import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import SailfishUiSet 1.0
import Nemo.Notifications 1.0
import "qrc:/qml/pages/browser/controls"
import "qrc:/qml/controls"

Page {
    id: pageRoot
    anchors.fill: parent

    property Component contextMenuComponent :
        Qt.createComponent(
            "qrc:/qml/pages/browser/controls/FileOperationsContextMenu.qml",
            Component.PreferSynchronous);

    property string remotePath : "/"
    property string pageHeaderText : "/"

    // Keep track of directory listing requests
    property var listCommand : null

    // Keep track of active dialog.
    // There can only be one of them active at a time due to their modal nature.
    property var dialogObj : null

    function __dialogCleanup() {
        //dialogObj.destroy()
        dialogObj = null
    }

    function refreshListView(refresh) {
        listCommand = browserCommandQueue.directoryListingRequest(remotePath, refresh)
    }

    FileDetailsHelper { id: fileDetailsHelper }
    Notification {
        id: transientNotifier
        isTransient: true
    }

    onStatusChanged: {
        if (status === PageStatus.Inactive) {
            if (_navigation !== undefined && _navigation === PageNavigation.Back) {
                if (listCommand !== null)
                    listCommand.abort()
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
        target: browserCommandQueue
        onCommandStarted: {
            // Get the handle to davList commands when remotePaths match
            var isDavListCommand = (command.info.property("type") === "davList")
            if (!isDavListCommand)
                return;

            // Don't replace existing handle
            if (!listCommand && command.info.property("remotePath") === remotePath) {
                listCommand = command;
            }
        }

        onCommandFinished: {
            // Invalidate listCommand after completion
            if (receipt.info.property("type") === "davList") {
                listCommand = null
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
        target: transferQueue
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
        // for the AvatarButton to adapt automatically
        height: userInformation.height
        //onHeightChanged: console.log(height)

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
                        label: qsTr("Total:")
                        value: ocsUserInfo.hrTotalBytes
                        visible: value.length > 0
                    }
                }

                Item {
                    width: (parent.width / 3)
                    height: width
                    AvatarButton {
                        source: avatarFetcher.source
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

    SilicaFlickable {
        id: flickContainer
        anchors.top: userInformationAnchor.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        SilicaListView {
            id: listView
            anchors.fill: parent
            clip: true
            model: directoryContents.value(remotePath)

            PullDownMenu {
                MenuItem {
                    text: qsTr("Refresh")
                    enabled: listView.model !== undefined
                    onClicked: {
                        refreshListView(true)
                    }
                }

                MenuItem {
                    text:qsTr("Create directory")
                    enabled: listView.model !== undefined
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
                            transferQueue.fileUploadRequest(selectedFiles[i].path,
                                                            canonicalRemotePath,
                                                            selectedFiles[i].lastModified);
                        }
                        transferQueue.run()
                        __dialogCleanup()
                    }

                    text: qsTr("Upload")
                    enabled: listView.model !== undefined
                    onClicked: {
                        dialogObj = selectionDialogComponent.createObject(pageRoot,
                                                                          {maximumSelections:Number.MAX_VALUE});
                        dialogObj.acceptText = qsTr("Upload")
                        dialogObj.errorOccured.connect(function(errorStr) {
                            transientNotifier.previewSummary = errorStr
                            transientNotifier.publish()
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
                        pageStack.push(transferPageComponent)
                    }
                }

                MenuItem {
                    text: qsTr("Settings")
                    onClicked: {
                        pageStack.push(settingsPageComponent)
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

                AvatarButton {
                    id: avatarButton
                    source: avatarFetcher.source
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

            delegate: ListItem {
                id: delegate

                property var davInfo : listView.model[index]

                Image {
                    id: icon
                    source: davInfo.isDirectory ?
                                "image://theme/icon-m-folder" :
                                fileDetailsHelper.getIconFromMime(davInfo.mimeType)
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.horizontalPageMargin
                    anchors.top: parent.top
                    anchors.topMargin: Theme.paddingSmall
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: Theme.paddingSmall
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: label
                    anchors.left: icon.right
                    anchors.leftMargin: Theme.paddingMedium
                    anchors.right: parent.right
                    text: davInfo.name
                    anchors.verticalCenter: parent.verticalCenter
                    color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                }

                onClicked: {
                    if(davInfo.isDirectory) {
                        var nextPath = remotePath + davInfo.name + "/";
                        listCommand = browserCommandQueue.directoryListingRequest(nextPath, false)
                    } else {
                        var fileDetails = fileDetailsComponent.createObject(pageRoot, { entry: davInfo });
                        pageStack.push(fileDetails);
                    }
                }

                // Delete menu resources after completion,
                // which MUST be indicated by the context menu component
                onPressAndHold: {
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
                }
            }
            VerticalScrollDecorator {}

            BusyIndicator {
                anchors.centerIn: parent
                running: listCommand !== null
                size: BusyIndicatorSize.Large
            }
        }
    }
}
