import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import "qrc:/qml/sfos/navigation"

Page {
    property AccountWorkerGenerator accountGenerator : null
    property Component browserPage : null

    property CommandEntity __listCommand : null

    SilicaListView {
        id: accountsList
        anchors.fill: parent
        model: accountGenerator.accountWorkers

        PullDownMenu {
            MenuItem {
                text: qsTr("Add account...")
                onClicked: {
                    pageStack.push(authenticationEntranceComponent)
                }
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("About")
            }
        }

        delegate: BackgroundItem {
            width: parent.width

            property var selectedAccountWorkers : accountsList.model[index]

            BrowserCommandPageFlow {
                id: browserPageFlow
                accountWorkers: selectedAccountWorkers
                onNotificationRequest: notify(summary, body)
                onTransientNotificationRequest: notifyTransient(summary)
                onAvatarFetchRequest: selectedAccountWorkers.avatarFetcher.fetch()
                onUserInfoUpdateRequest: selectedAccountWorkers.accountInfoCommandQueue.userInfoRequest()
            }

            Label {
                width: parent.width
                text: selectedAccountWorkers.account.username + "@" +
                      selectedAccountWorkers.account.hoststring
            }

            onClicked: {
                var nextPath = "/";
                __listCommand = selectedAccountWorkers.browserCommandQueue.directoryListingRequest(nextPath, false)
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: __listCommand != null
    }
}
