import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import harbour.owncloud 1.0
import "qrc:/qml/qqc/dialogs"
import "qrc:/qml/qqc/pages"
import "qrc:/qml-ui-set"

ApplicationWindow {
    id: rootWindow
    objectName: "rootWindow"
    title: "QhostCloud"
    visible: true
    width: 600
    minimumWidth: 260
    height: 400
    minimumHeight: 360

    // TODO: save window geometry before closing

    readonly property int fontSizeTiny : 10
    readonly property int fontSizeSmall : 14
    readonly property int fontSizeMedium : 18
    readonly property int fontSizeLarge : 26

    readonly property int paddingTiny : 8
    readonly property int paddingSmall : 16
    readonly property int paddingMedium : 20
    readonly property int paddingLarge : 32

    readonly property alias pageStack : rootStack
    readonly property alias detailsStack : sideStack

    readonly property bool detailStackVisibleRequired :
        (sideStack.currentItem !== sideStack.initialItem) &&
        (sideStack.currentItem !== null)

    readonly property bool sideStackIsActive : {
        if (width > height) {
            return true;
        } else {
            return false;
        }
    }

    readonly property alias dirContents : directoryContents

    property int currentAccountIndex : 0

    readonly property var selectedAccount :
        accountWorkerGenerator.accountWorkers.length > 0
        ? accountWorkerGenerator.accountWorkers[currentAccountIndex]
        : null

    readonly property bool showBackButton :
        (rootStack.initialItem !== rootStack.currentItem) &&
        (rootStack.currentItem !== null) ||
        (sideStack.initialItem !== sideStack.currentItem) &&
        (sideStack.currentItem !== null)

    function refreshUserInfo() {
        // Skip in case there's already a userInfo command in the queue
        for (var i = 0; i < accountInfoCommandQueue.queue.length; i++) {
            var commandInfo = accountInfoCommandQueue.queue[i].info;
            if (commandInfo.type === "userInfo")
                return;
        }

        //        if (transferQueue.queue.length < 1) {
        //            selectedAccount.accountInfoCommandQueue.userInfoRequest();
        //            selectedAccount.accountInfoCommandQueue.run()
        //        }
    }
    function notify(summary, body) {
        msgDialog.showMessage(summary, body)
    }

    function notifyTransient(summary) {
        msgDialog.showMessage(summary, "")
    }

    function popPage()  {
        if (sideStack.currentItem !== sideStack.initialItem) {
            sideStack.pop()
            return true;
        }

        if (rootStack.currentItem !== rootStack.initialItem) {
            rootStack.pop()
            return true;
        }

        return false
    }

    function addAccount() {
        // TODO: check if memory leaks happen here
        detailsStack.push(webDavAccountDialog)
    }

    header: ToolBar {
        id: headerBar
        height: 64
        width: rootWindow.width
        RowLayout {
            anchors.fill: parent
            ToolButton {
                icon.color: "transparent"
                icon.name: "back"
                font.pixelSize: fontSizeSmall
                enabled: showBackButton
                onClicked: popPage()
            }

            CircularImageButton {
                id: avatarButton
                visible: rootStack.currentItem.objectName === "FileBrowser" &&
                         sideStack.currentItem === sideStack.initialItem
                height: parent.height
                width: height
                shadowEnabled: false
                imageBackgroundColor: "black"
                imageBackgroundEnabled: true
                source: {
                    if (rootStack.currentItem.objectName == "FileBrowser") {
                        return rootStack.currentItem.accountWorkers.avatarFetcher.source
                    } else {
                        return ""
                    }
                }

                onClicked: {
                    if (rootStack.currentItem === undefined)
                        return
                    if (rootStack.currentItem === null)
                        return

                    rootStack.currentItem.avatarMenu.open()
                }
            }

            Label {
                font.pixelSize: fontSizeSmall
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
                text: {
                    if (sideStack.currentItem !== sideStack.initialItem &&
                               sideStack.currentItem.title !== undefined) {
                        return sideStack.currentItem.title
                    } else if (rootStack.currentItem !== sideStack.initialItem &&
                               rootStack.currentItem.title !== undefined) {
                        return rootStack.currentItem.title
                    } else {
                        return ""
                    }
                }
            }

            ToolButton {
                icon.color: "transparent"
                icon.name: "folder-new"
                visible: rootStack.currentItem.objectName === "FileBrowser" &&
                         sideStack.currentItem === sideStack.initialItem
                font.pixelSize: fontSizeSmall
                enabled: !rootStack.currentItem.isBusy
                onClicked: {
                    rootStack.currentItem.newDirectoryDialog.open()
                }
            }
            ToolButton {
                icon.color: "transparent"
                icon.name: "document-new"
                visible: rootStack.currentItem.objectName === "FileBrowser" &&
                         sideStack.currentItem === sideStack.initialItem
                font.pixelSize: fontSizeSmall
                enabled: !rootStack.currentItem.isBusy
                onClicked: {
                    rootStack.currentItem.fileUploadDialog.open()
                }
            }
            ToolButton {
                icon.color: "transparent"
                icon.name: "view-refresh"
                visible: rootStack.currentItem.objectName === "FileBrowser" &&
                         sideStack.currentItem === sideStack.initialItem
                font.pixelSize: fontSizeSmall
                enabled: !rootStack.currentItem.isBusy
                onClicked: {
                    rootStack.currentItem.refreshListView(true)
                }
            }
            ToolButton {
                icon.color: "transparent"
                icon.name: "application-menu"
                onClicked: {
                    if (hamburgerMenu.opened) {
                        hamburgerMenu.close()
                    } else {
                        hamburgerMenu.open()
                    }
                }
            }
        }
    }

    Item {
        anchors.fill: parent
        focus: true

        AccountWorkerGenerator {
            id: accountWorkerGenerator
            database: AccountDb { }
        }

        DaemonControl {
            id: daemonCtrl
        }

        QmlMap {
            id: directoryContents
        }

        MessageDialog {
            function showMessage(summary, body) {
                msgDialog.text = summary
                msgDialog.informativeText = body
                msgDialog.open()
            }

            id: msgDialog
            text: ""
        }

        Item {
            anchors.right: parent.right
            anchors.top: parent.top

            Menu {
                id: hamburgerMenu

                MenuItem {
                    text: qsTr("Transfers")
                    font.pixelSize: fontSizeSmall
                    onClicked: {
                        detailsStack.push(transfersTab)
                    }
                }
                MenuItem {
                    text: qsTr("Add account")
                    font.pixelSize: fontSizeSmall
                    visible: (accountWorkerGenerator.accountWorkers.length > 0)
                    onClicked: addAccount()
                }

                MenuSeparator {}

                MenuItem {
                    text: qsTr("About QhostCloud")
                    font.pixelSize: fontSizeSmall
                    visible: true
                    onClicked: {
                        detailsStack.push(infoPage)
                    }
                }
            }
        }

        StackLayout {
            id: tabLayout
            currentIndex: 0

            Rectangle {
                id: homeTab
                objectName: "entrancePage"
                Layout.alignment: Qt.AlignCenter

                AccountSelection {
                    id: accountSelection
                    anchors.fill: parent
                    accountGenerator: accountWorkerGenerator
                    visible: accountWorkerGenerator.accountWorkers.length > 0
                    dirContents : rootWindow.dirContents
                    detailStack: detailsStack
                }

                Column {
                    anchors.centerIn: parent
                    width: parent.width
                    visible: accountWorkerGenerator.accountWorkers.length < 1

                    Label {
                        font.pixelSize: 32
                        width: parent.width
                        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("No account available yet. " +
                                   "Please add an account to contiune.")
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("Add account")
                        onClicked: addAccount()
                    }
                }
            }

            WebDavAccountDialog {
                id: webDavAccountDialog
                accountWorkers: accountWorkerGenerator.newAccount();
                daemonCtrl: daemonCtrl
                accountDatabase: accountWorkerGenerator.database
                viewStack: detailsStack
            }

            About {
                id: infoPage
                onCloseRequest: detailsStack.pop()
            }

            TransferPage {
                id: transfersTab
                accountGenerator: accountWorkerGenerator
                onCloseRequest: detailsStack.pop()
            }
        }

        SwipeView {
            anchors.fill: parent
            states: [
                State {
                    when: sideStackIsActive
                    ParentChange {
                        target: rootStack
                        parent: rootStackContainer
                    }
                    ParentChange {
                        target: sideStack
                        parent: sideStackContainer
                    }
                },
                State {
                    when: !sideStackIsActive
                    ParentChange {
                        target: rootStack
                        parent: rootStackContainer
                    }
                    ParentChange {
                        target: sideStack
                        parent: rootStackContainer
                    }
                }
            ]

            Item {
                StackView {
                    id: rootStack
                    initialItem: tabLayout
                    anchors.fill: parent

                    onPopExitChanged: {
                        gc()
                    }
                }
            }

            Item {
                StackView {
                    id: sideStack
                    anchors.fill: parent
                    initialItem: Rectangle {
                        color: "transparent"
                        Image {
                            id: icon
                            source: "qrc:/icons/icon_gray.svg"
                            visible: sideStackIsActive
                            anchors.centerIn: parent
                            width: parent.width / 2
                            height: width
                            scale: 1.5
                            sourceSize.width: width
                            sourceSize.height: height
                        }
                    }
                    onPushEnterChanged: {
                        if (detailStackVisibleRequired) {
                            sideStack.pop()
                        }
                    }
                }
            }
        }

        Rectangle {
            id: mainContainer
            color: "lightgray"
            anchors.fill: parent

            Row {
                id: splitView
                anchors.fill: parent
                spacing: 2
                Rectangle {
                    id: rootStackContainer
                    width: sideStackIsActive ? (parent.width / 3)
                                             : parent.width
                    height: parent.height
                    clip: true
                }
                Rectangle {
                    id: sideStackContainer
                    width: sideStackIsActive ? ((parent.width / 3) * 2)
                                             : parent.width
                    height: parent.height
                    visible: sideStackIsActive || detailStackVisibleRequired
                    clip: true
                }
            }
        }
    }

    Shortcut {
        sequence: StandardKey.Back
        onActivated: {
            if (popPage()) {
                return
            }
            Qt.quit()
        }
    }
}
