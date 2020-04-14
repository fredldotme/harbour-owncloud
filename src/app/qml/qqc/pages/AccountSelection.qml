import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import harbour.owncloud 1.0
import "qrc:/qml/qqc"
import "qrc:/qml-ui-set"

Item {
    id: accountsSelectionRoot

    property AccountWorkerGenerator accountGenerator : null
    property Component browserPage : null
    property QmlMap dirContents : null
    property StackView detailStack : null
    property bool avatarMenuOpen : false

    readonly property Component settingsPageComponent :
        Qt.createComponent("qrc:/qml/qqc/pages/SettingsPage.qml",
                           Component.PreferSynchronous);

    onDirContentsChanged: {
        console.log("AccountSelection.dirContents: " + dirContents)
    }

    readonly property var providerTypeNames : ["Nextcloud/ownCloud", "WebDav"]

    property CommandEntity __listCommand : null

    property var selectedAccountWorkers : null

    BrowserCommandPageFlow {
        id: pageFlow
        parent: splitView
        anchors.centerIn: parent
        accountWorkers: selectedAccountWorkers
        directoryContents: accountsSelectionRoot.dirContents
        detailsStack: detailStack
        onNotificationRequest: notify(summary, body)
        onTransientNotificationRequest: notifyTransient(summary)
        onAvatarFetchRequest: selectedAccountWorkers.avatarFetcher.fetch()
        onUserInfoUpdateRequest: {
            selectedAccountWorkers.accountInfoCommandQueue.userInfoRequest()
            selectedAccountWorkers.accountInfoCommandQueue.run()
        }
    }

    Menu {
        id: rightClickMenu

        MenuItem {
            text: qsTr("Settings...")
            font.pixelSize: fontSizeSmall
            onClicked: {
                var settingsPage = settingsPageComponent.createObject(detailStack,
                                                                      {
                    accountDb : accountGenerator.database,
                    accountWorkers : selectedAccountWorkers
                                                                      });
                detailStack.push(settingsPage)
            }
        }
        MenuItem {
            text: qsTr("Remove account")
            font.pixelSize: fontSizeSmall
            onClicked: {
                console.log("Removing " + selectedAccountWorkers.account)
                var deleteSuccess =
                        accountGenerator.database.removeAccount(selectedAccountWorkers.account)
                console.log("deleteSuccess: " + deleteSuccess)
            }
        }
    }

    ListView {
        id: accountsList
        anchors.fill: parent
        model: accountGenerator.accountWorkers
        ScrollBar.vertical: ScrollBar {}
        spacing: 8

        delegate: MouseArea {
            id: accountMouseArea
            width: childrenRect.width
            height: childrenRect.height
            enabled: (__listCommand == null) &&
                     (detailStack.currentItem.objectName !== "settingsPage")
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            readonly property var delegateAccountWorkers : accountGenerator.accountWorkers[index]

            function openContextMenu(x, y, newParent) {
                rightClickMenu.parent = newParent
                rightClickMenu.x = x
                rightClickMenu.y = y
                rightClickMenu.open()
            }

            onPressAndHold: {
                selectedAccountWorkers = delegateAccountWorkers
                openContextMenu(mouseX, mouseY, accountsList)
            }

            onClicked: {
                selectedAccountWorkers = delegateAccountWorkers

                if (mouse.button === Qt.RightButton) {
                    openContextMenu(mouseX, mouseY, accountsList)
                    return;
                }

                var nextPath = "/";
                pageFlow.targetRemotePath = nextPath
                __listCommand = selectedAccountWorkers.browserCommandQueue.directoryListingRequest(nextPath, false)

                console.debug("onClicked: __listCommand " + __listCommand)
            }

            Rectangle {
                anchors.fill: mainColumn
                color: {
                    if (accountMouseArea.pressed)
                        return "lightgray"
                    else
                        return "transparent"
                }
            }

            Column {
                id: mainColumn
                Label {
                    id: providerLabel
                    text: providerTypeNames[delegateAccountWorkers.account.providerType]
                    font.bold: true
                    font.pixelSize: fontSizeMedium
                    enabled: __listCommand == null
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    width: accountsList.width
                }

                Label {
                    id: userLabel
                    text: qsTr("%1 on %2", "username on https://server:443/dav/").arg(delegateAccountWorkers.account.username).arg(delegateAccountWorkers.account.hoststring)
                    font.pixelSize: fontSizeMedium
                    enabled: __listCommand == null
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    width: accountsList.width
                }
            }
        }
    }

    AbortableBusyIndicator {
        anchors.centerIn: parent
        running: __listCommand != null
        buttonVisibiltyDelay: 5000
        onAbort: {
            if (!__listCommand)
                return
            __listCommand.abort(true)
        }
    }
}
