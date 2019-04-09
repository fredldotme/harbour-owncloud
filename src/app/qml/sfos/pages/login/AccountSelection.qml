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
    readonly property var providerTypeNames : ["Nextcloud/ownCloud", "WebDav"]

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

        delegate: ListItem {
            width: ListView.view.width
            //height: mainColumn.height

            property var delegateAccountWorkers : accountsList.model[index]

            Column {
                id: mainColumn
                width: parent.width
                Label {
                    id: providerLabel
                    text: providerTypeNames[delegateAccountWorkers.account.providerType]
                    font.bold: true
                    font.pixelSize: Theme.fontSizeSmall
                    enabled: __listCommand == null
                    width: accountsList.width
                }

                Label {
                    id: userLabel
                    text: delegateAccountWorkers.account.username
                          + " on "
                          + delegateAccountWorkers.account.hoststring
                    font.pixelSize: Theme.fontSizeSmall
                    enabled: __listCommand == null
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    width: accountsList.width
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
                    text: qsTr("Remove account")
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

            onPressAndHold: {
                // TODO: "Remove account" button
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
