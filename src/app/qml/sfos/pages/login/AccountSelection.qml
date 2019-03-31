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
                onClicked: {
                    pageStack.push(aboutPageComponent)
                }
            }
        }

        delegate: BackgroundItem {
            width: parent.width

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
