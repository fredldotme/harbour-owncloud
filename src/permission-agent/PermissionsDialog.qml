import QtQuick 2.2
import Sailfish.Silica 1.0

ApplicationWindow
{
    id: dialogRoot
    cover: undefined

    Connections {
        target: queue
        onPermissionRequested: pageStack.openDialog(dialog)
    }

    initialPage: Component {
        Page {
            SilicaFlickable { }
        }
    }

    Dialog {
        id: dialog

        DialogHeader {
            title: qsTr("Do you want to grant temporary access to your Nextcloud/ownCloud?")
        }

        onAccepted: {
            dialogRoot.deactivate()
            queue.permitFirstInQueue()
        }
        onRejected: {
            dialogRoot.deactivate()
            queue.discardFirstInQueue()
        }
    }
}
