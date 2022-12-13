import QtQuick 2.0
import Lomiri.Components 1.1
import Lomiri.Content 1.1

Item {
    id: dialogRoot
    anchors.fill: parent
    visible: false

    property list<ContentItem> importItems
    property var activeTransfer
    property var fileUrls : new Array

    onImportItemsChanged: {
        if (importItems.length > 0) {
            fileUrls = new Array

            for (var i = 0; i < importItems.length; i++) {
                const url = importItems[i].url
                console.log("URL: " + url)
                if (fileUrls.indexOf(url) == -1) {
                    fileUrls.push(url)
                }
            }

            dialogRoot.accepted()
            dialogRoot.visible = false
        }
    }

    signal accepted()

    function open() {
        dialogRoot.visible = true
    }

    ContentPeerPicker {
        id: contentPeer
        anchors.fill: parent
        contentType: ContentType.Documents
        handler: ContentHandler.Source
        onPeerSelected: { activeTransfer = peer.request() }
        onCancelPressed: { dialogRoot.visible = false }
    }

    ContentTransferHint {
        id: importHint
        anchors.fill: parent
        activeTransfer: dialogRoot.activeTransfer
    }

    Connections {
        target: dialogRoot.activeTransfer
        onStateChanged: {
            if (dialogRoot.activeTransfer.state === ContentTransfer.Charged) {
                importItems = dialogRoot.activeTransfer.items;
            }
        }
    }
}
