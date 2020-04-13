import QtQuick 2.2
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

ApplicationWindow
{
    id: dialogRoot
    cover: undefined

    Connections {
        target: queue
        onPermissionRequested: {
            accountDb.refresh()
            pageStack.openDialog(dialog)
        }
    }

    initialPage: Component {
        Page {
            SilicaFlickable { }
        }
    }

    Dialog {
        id: dialog

        property var selectedAccount : null
        canAccept: selectedAccount !== null

        AccountWorkerGenerator {
            id: accountWorkerGenerator
            database: AccountDb {
                id: accountDb
            }
        }

        DialogHeader {
            id: header
            title: qsTr("Which cloud storage account would you like to use?")
        }

        ComboBox {
            id: accountsList
            width: parent.width
            anchors {
                top: header.bottom
                left: parent.left
                right: parent.right
            }
            onCurrentIndexChanged: {
                if (currentIndex === 0) {
                    dialog.selectedAccount = null
                }
            }

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Select an account...")
                }

                Repeater {
                    id: accountRepeater
                    model: accountWorkerGenerator.accountWorkers
                    delegate: MenuItem {
                        property var delegateAccountWorkers : accountRepeater.model[index]
                        text: qsTr("%1 on %2", "username on https://server:443/dav/").arg(delegateAccountWorkers.account.username).arg(delegateAccountWorkers.account.hoststring)
                        onClicked: {
                            dialog.selectedAccount = delegateAccountWorkers.account
                        }
                    }
                }
            }
        }

        onAccepted: {
            dialogRoot.deactivate()
            queue.permitFirstInQueue(selectedAccount)
            accountsList.currentIndex = 0
        }
        onRejected: {
            dialogRoot.deactivate()
            queue.discardFirstInQueue()
            accountsList.currentIndex = 0
        }
    }
}
