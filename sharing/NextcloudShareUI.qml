import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.thumbnailer 1.0
import Sailfish.TransferEngine 1.0

ShareDialog {
    id: root

    property int viewWidth: root.isPortrait ? Screen.width : Screen.width / 2

    onAccepted: {
        shareItem.start()
    }

    Thumbnail {
        id: thumbnail
        width: viewWidth
        height: parent.height / 2
        source: root.source
        sourceSize.width: Screen.width
        sourceSize.height: Screen.height / 2
    }

    Item {
        anchors {
            top: root.isPortrait ? thumbnail.bottom : parent.top
            left: root.isPortrait ? parent.left: thumbnail.right
            right: parent.right
            bottom: parent.bottom
        }

        Label {
            anchors.centerIn:parent
            width: viewWidth
            text: qsTrId("example-test-share-ui-la-id")
            horizontalAlignment: Text.AlignHCenter
        }
    }

    SailfishShare {
        id: shareItem
        source: root.source
        metadataStripped: true
        serviceId: root.methodId
        userData: { "description": "a description" }
    }

    DialogHeader {
        acceptText: qsTrId("Accept")
    }
}

