import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import "qrc:/qml/sfos/navigation"
import "qrc:/sailfish-ui-set"

Page {
    id: accountsSelectionRoot
    property AccountWorkerGenerator accountGenerator : null
    property Component browserPage : null
    property QmlMap dirContents : null

    property CommandEntity __listCommand : null
    property var selectedAccountWorkers : null
    readonly property var providerTypeNames : [
        "Nextcloud/ownCloud",
        "WebDav"
    ]

    BrowserCommandPageFlow {
        id: pageFlow
        anchors.centerIn: parent
        accountWorkers: selectedAccountWorkers
        directoryContents: accountsSelectionRoot.dirContents
        onNotificationRequest: notify(summary, body)
        onTransientNotificationRequest: notifyTransient(summary)
        onAvatarFetchRequest: selectedAccountWorkers.avatarFetcher.fetch()
        onUserInfoUpdateRequest: {
            selectedAccountWorkers.accountInfoCommandQueue.userInfoRequest()
            selectedAccountWorkers.accountInfoCommandQueue.run()
        }
    }

    SilicaListView {
        id: accountsList
        anchors.fill: parent
        model: accountGenerator.accountWorkers

        PullDownMenu {
            MenuItem {
                text: qsTr("Add account...")
                enabled: __listCommand == null
                onClicked: {
                    // TODO: Flow authentication
                    // pageStack.push(authenticationEntranceComponent)
                    var basicAuthPage =
                            basicAuthenticationComponent
                    .createObject(accountsSelectionRoot,
                                  {
                                      accountWorkers: accountWorkerGenerator.newAccount(),
                                      daemonCtrl: daemonCtrl,
                                      accountDatabase: accountWorkerGenerator.database
                                  });
                    pageStack.push(basicAuthPage)
                }
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("About")
                enabled: __listCommand == null
                onClicked: {
                    pageStack.push(aboutPageComponent)
                }
            }
        }

        header: PageHeader {
            title: "GhostCloud"
        }

        delegate: ListItem {
            width: ListView.view.width
            enabled: __listCommand === null
            contentHeight: mainColumn.height

            property var delegateAccountWorkers : accountsList.model[index]

            Item {
                id: mainColumn
                width: parent.width
                height: providerLabel.height + userLabel.height + 2.0*Theme.paddingSmall

                Label {
                    id: providerLabel
                    text: providerTypeNames[delegateAccountWorkers.account.providerType]
                    font.bold: true
                    font.pixelSize: Theme.fontSizeMedium
                    enabled: __listCommand == null
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                        topMargin: Theme.paddingSmall
                        leftMargin: Theme.paddingSmall
                        bottomMargin: Theme.paddingSmall
                    }
                }

                Label {
                    id: userLabel
                    text: qsTr("%1 on %2", "username on https://server:443/dav/").arg(delegateAccountWorkers.account.username).arg(delegateAccountWorkers.account.hoststring)
                    font.pixelSize: Theme.fontSizeSmall
                    enabled: __listCommand == null
                    elide: Label.ElideRight
                    anchors {
                        top: providerLabel.bottom
                        left: parent.left
                        right: parent.right
                        leftMargin: Theme.paddingSmall
                        rightMargin: Theme.paddingSmall
                        bottomMargin: Theme.paddingSmall
                    }
                }
            }

            Connections {
                id: transferCommandQueueNotify
                target: delegateAccountWorkers.transferCommandQueue
                onCommandFinished: {
                    // Ignore invalid CommandReceipts
                    if (!receipt.valid)
                        return;

                    // Ignore if the command was intentionally aborted by the user
                    if (receipt.abortIntended)
                        return

                    var transferType = receipt.info.property("type")
                    console.log("transferType " + transferType)
                    var fileName = receipt.info.property("fileName")

                    // Notify the user of successful or erroneous file transfers.
                    // Also refresh the userInfo in case an upload was successful.
                    if (transferType === "fileUpload") {
                        if (receipt.finished) {
                            notify(qsTr("Upload complete"), qsTr("%1 uploaded successfully").arg(fileName))
                            refreshUserInfo()
                        } else {
                            notify(qsTr("Upload failed!"), qsTr("%1 couldn't be uploaded").arg(fileName))
                        }
                    } else if (transferType === "fileDownload") {
                        if (receipt.finished)
                            notify(qsTr("Download complete"), qsTr("%1 downloaded successfully").arg(fileName))
                        else
                            notify(qsTr("Download failed!"), qsTr("%1 couldn't be downloaded").arg(fileName))
                    }
                }
            }

            RemorseItem {
                id: removeRemorse
            }

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Settings")
                    enabled: __listCommand === null
                    onClicked: {
                        var settingsPage =
                                settingsPageComponent.createObject(accountsSelectionRoot,
                                                                   {
                                                                       accountWorkers : delegateAccountWorkers,
                                                                       accountDb: accountWorkerGenerator.database
                                                                   });
                        pageStack.push(settingsPage)
                    }
                }
                MenuItem {
                    text: qsTr("Remove account")
                    enabled: __listCommand === null
                    onClicked: {
                        removeRemorse.execute(mainColumn,
                                              qsTr("Removing account"),
                                              function(){
                                                  accountWorkerGenerator.database.removeAccount(delegateAccountWorkers.account)
                                              },
                                              5000);
                    }
                }
            }

            onClicked: {
                selectedAccountWorkers = delegateAccountWorkers

                if (mouse.button === Qt.RightButton) {
                    openContextMenu(mouseX, mouseY, accountsList)
                    return;
                }

                var nextPath = "/";
                pageFlow.targetRemotePath = nextPath
                ocsUserInfo.commandQueue = selectedAccountWorkers.accountInfoCommandQueue
                __listCommand = selectedAccountWorkers.browserCommandQueue.directoryListingRequest(nextPath, false)

                console.debug("onClicked: __listCommand " + __listCommand)
            }
        }
    }

    ViewPlaceholder {
        text: qsTr("No accounts available. Please add an account to continue.")
        visible: accountGenerator.accountWorkers.length < 1
        enabled: visible
    }

    AbortableBusyIndicator {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: __listCommand != null
        onAbort: {
            if (!__listCommand)
                return
            __listCommand.abort(true)
        }
    }
}
