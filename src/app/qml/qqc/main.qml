import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import harbour.owncloud 1.0
import "qrc:/qml/qqc"
import "qrc:/qml/qqc/controls"
import "qrc:/qml/qqc/dialogs"
import "qrc:/qml/qqc/pages"
import "qrc:/qml-ui-set"

ApplicationWindow {
    id: rootWindow
    objectName: "rootWindow"
    title: "GhostCloud"
    visible: true
    width: 600
    minimumWidth: 320
    height: 400
    minimumHeight: 380

    readonly property int fontSizeTiny : 10
    readonly property int fontSizeSmall : 14
    readonly property int fontSizeMedium : 16
    readonly property int fontSizeLarge : 26

    readonly property int paddingTiny : 8
    readonly property int paddingSmall : 16
    readonly property int paddingMedium : 20
    readonly property int paddingLarge : 32

    readonly property bool osIsAndroid : targetOs === 1
    readonly property bool osIsIOS : targetOs === 2
    readonly property bool osIsUbuntuTouch : targetOs === 3
    readonly property bool osIsMacOs : targetOs === 4

    readonly property alias pageStack : rootStack
    readonly property alias detailsStack : sideStack

    readonly property bool detailStackVisibleRequired :
        (sideStack.depth > 1)

    readonly property bool sideStackIsActive : {
        if (width > height && detailStackVisibleRequired) {
            return true;
        } else {
            return false;
        }
    }
    onSideStackIsActiveChanged: {
        if (sideStackIsActive) {
            pullInAnimation.stop();
            pullOutAnimation.start()
        } else {
            pullOutAnimation.stop();
            pullInAnimation.start();
        }
    }

    // HACK: Refresh account db on Ubuntu Touch when focus changes.
    //       This needs to be done until the Online Accounts Daemon
    //       account enable/disable signal bug is fixed
    onActiveChanged: {
        if (!osIsUbuntuTouch)
            return;
        if (sideStack.currentItem !== sideStack.initialItem)
            return;
        if (!active)
            return;

        accountsDb.refresh()
    }

    readonly property alias dirContents : directoryContents

    property int currentAccountIndex : 0

    readonly property var selectedAccount :
        accountWorkerGenerator.accountWorkers.length > 0
        ? accountWorkerGenerator.accountWorkers[currentAccountIndex]
        : null

    readonly property bool showBackButton :
        (rootStack.depth > 1) &&
        (rootStack.currentItem !== null) ||
        (sideStack.depth > 1) &&
        (sideStack.currentItem !== null)

    function refreshUserInfo() {
        // Skip in case there's already a userInfo command in the queue
        for (var i = 0; i < accountInfoCommandQueue.queue.length; i++) {
            var commandInfo = accountInfoCommandQueue.queue[i].info;
            if (commandInfo.type === "userInfo")
                return;
        }

        if (transferQueue.queue.length < 1) {
            accountInfoCommandQueue.userInfoRequest();
            accountInfoCommandQueue.run()
        }
    }
    function notify(summary, body) {
        msgDialog.showMessage(summary, body)
    }

    function notifyTransient(summary) {
        msgDialog.showMessage(summary, "")
    }

    function popAndTryDestroy() {
        var page = sideStack.pop()
        if (!page)
            return true

        if (page.destroyable !== undefined && page.destroyable) {
            page.destroy()
        }
        return true;
    }

    function popPage()  {
        if (sideStack.currentItem !== sideStack.initialItem) {
            return popAndTryDestroy()
        }

        if (rootStack.currentItem !== rootStack.initialItem) {
            var page = rootStack.pop()
            if (page.destroyable !== undefined && page.destroyable) {
                page.destroy()
            }
            return true;
        }

        return false
    }

    function addAccount() {
        // TODO: check if memory leaks happen here
        //detailsStack.push(webDavAccountDialog)
        if (osIsUbuntuTouch) {
            Qt.openUrlExternally("settings:///online-accounts")
        } else {
            webDavAccountDialog.open()
        }
    }

    // Android back button support
    onClosing: {
        if (!osIsAndroid) {
            return
        }

        if (showBackButton) {
            popPage()
            close.accepted = false
        } else {
            return
        }
    }

    function getActionBarIcon(icon) {
        if (osIsUbuntuTouch) {
            if (icon === "application-menu") {
                return "image://theme/navigation-menu"
            }

            return "image://theme/" + icon
        } else {
            if (icon === "list-add") {
                return "qrc:/icons/theme/actions/24/" + icon + ".svg"
            }

            return "qrc:/icons/theme/actions/32/" + icon + ".svg"
        }
    }

    function getFolderIcon(icon) {
        if (osIsUbuntuTouch) {
            return "image://theme/" + icon
        } else {
            return "qrc:/icons/theme/places/64/" + icon + ".svg"
        }
    }

    Settings {
        id: windowSettings
        category: "windowProperties"
        property alias width: rootWindow.width
        property alias height: rootWindow.height
        property alias x: rootWindow.x
        property alias y: rootWindow.y
    }

    header: ToolBar {
        id: headerBar
        height: headerBarSize
        width: rootWindow.width
        background: Rectangle { color: "white" }
        RowLayout {
            anchors.fill: parent
            GCButton {
                source: getActionBarIcon("go-previous")
                visible: showBackButton
                onClicked: popPage()
                height: parent.height
                width: height
                anchors.margins: paddingSmall
                sourceSize.width: width
                sourceSize.height: height
            }

            GCButton {
                id: avatarButton
                visible: rootStack.currentItem.objectName === "FileBrowser" &&
                         sideStack.depth == 1
                height: parent.height
                width: height
                forcePressed: rootStack.currentItem.avatarMenuOpen !== undefined &&
                              rootStack.currentItem.avatarMenuOpen
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

                    rootStack.currentItem.openAvatarMenu()
                }
            }

            Label {
                font.pixelSize: fontSizeSmall
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                height: parent.height
                Layout.fillWidth: true
                text: {
                    if (sideStack.depth > 1 &&
                            sideStack.currentItem.title !== undefined) {
                        return sideStack.currentItem.title
                    } else if (rootStack.currentItem.title !== undefined) {
                        return rootStack.currentItem.title
                    } else {
                        return ""
                    }
                }
            }

            GCButton {
                source: getActionBarIcon("view-refresh")
                visible: rootStack.currentItem.objectName === "FileBrowser" &&
                         sideStack.depth == 1
                enabled: !rootStack.currentItem.isBusy
                height: parent.height
                anchors.margins: paddingSmall
                width: height
                onClicked: {
                    rootStack.currentItem.refreshListView(true)
                }
            }
            GCButton {
                source: getActionBarIcon("list-add")
                visible: rootStack.currentItem.objectName === "FileBrowser" &&
                         sideStack.depth == 1
                enabled: !rootStack.currentItem.isBusy
                height: parent.height
                width: height
                anchors.margins: paddingSmall
                forcePressed: addMenu.visible
                onClicked: {
                    addMenu.open(rootWindow)
                }
            }
            GCButton {
                source: getActionBarIcon("application-menu")
                height: parent.height
                width: height
                anchors.margins: paddingSmall
                forcePressed: hamburgerMenu.visible
                onClicked: {
                    hamburgerMenu.open(rootWindow)
                }
            }
        }
    }

    AccountWorkerGenerator {
        id: accountWorkerGenerator
        database: accountsDb
    }

    DaemonControl {
        id: daemonCtrl
    }

    QmlMap {
        id: directoryContents
    }

    Item {
        id: dialogLayer
        anchors.fill: parent

        Dialog {
            function showMessage(summary, body) {
                msgDialog.title = summary
                msgDialogText.text = body
                msgDialog.open()
            }

            id: msgDialog
            standardButtons: Dialog.Ok
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            Label {
                id: msgDialogText
                width: parent.width
            }
        }

        WebDavAccountDialog {
            id: webDavAccountDialog
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: parent.width - (paddingLarge * 2)
            height: parent.height - (paddingLarge * 2)
            accountWorkers: accountWorkerGenerator.newAccount();
            daemonCtrl: daemonCtrl
            accountDatabase: accountWorkerGenerator.database
            viewStack: detailsStack
        }

        TransferPage {
            id: transfersTab
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: parent.width - (paddingLarge * 2)
            height: parent.height - (paddingLarge * 2)
            accountGenerator: accountWorkerGenerator
        }

        About {
            id: infoPage
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: parent.width - (paddingLarge * 2)
            height: parent.height - (paddingLarge * 2)
        }

        Menu {
            id: addMenu
            x: rootWindow.width - width

            MenuItem {
                text: qsTr("Upload file...")
                font.pixelSize: fontSizeSmall
                onClicked: {
                    if (osIsAndroid) {
                        rootStack.currentItem.openNativeFileSelector()
                        return
                    }

                    rootStack.currentItem.fileUploadDialog.open()
                }
            }
            MenuItem {
                text: qsTr("New folder...")
                font.pixelSize: fontSizeSmall
                visible: (accountWorkerGenerator.accountWorkers.length > 0)
                onClicked: rootStack.currentItem.dirCreationDialog.open()
            }
        }

        Menu {
            id: hamburgerMenu
            x: rootWindow.width - width

            MenuItem {
                text: qsTr("Transfers")
                font.pixelSize: fontSizeSmall
                onClicked: {
                    transfersTab.open()
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
                text: qsTr("About GhostCloud")
                font.pixelSize: fontSizeSmall
                visible: true
                onClicked: {
                    infoPage.open()
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
                    spacing: 8

                    Text {
                        font.pixelSize: 24
                        width: parent.width
                        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.margins: paddingSmall
                        text: qsTr("No account available yet. " +
                                   "Please add an account to continue.")
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("Add account")
                        onClicked: addAccount()
                    }
                }
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
                        if (sideStack.depth > 1) {
                            popAndTryDestroy();
                        }
                    }
                }
            }
        }

        Rectangle {
            id: mainContainer
            color: "lightgray"
            anchors.fill: parent
            anchors.bottomMargin: Qt.inputMethod.visible ?
                                      Qt.inputMethod.keyboardRectangle.height / (GRID_UNIT_PX / 8) : 0

            Row {
                id: splitView
                anchors.fill: parent
                anchors.topMargin: 1
                spacing: 1
                focus: true

                property real pullOutFactor : 0.0
                readonly property int __pullOutDuration : 200

                NumberAnimation {
                    id: pullInAnimation
                    target: splitView
                    property: "pullOutFactor"
                    duration: splitView.__pullOutDuration
                    from: 1.0
                    to: 0.0
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    id: pullOutAnimation
                    target: splitView
                    property: "pullOutFactor"
                    duration: splitView.__pullOutDuration
                    from: 0.0
                    to: 1.0
                    easing.type: Easing.InOutQuad
                }

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
                    scale: splitView.pullOutFactor
                    opacity: splitView.pullOutFactor
                    height: parent.height
                    visible: sideStackIsActive
                    clip: true
                }
            }
        }
    }
}
