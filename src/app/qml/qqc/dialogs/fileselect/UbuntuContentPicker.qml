import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Content 0.1

Item {
    id: dialogRoot
    property list<ContentItem> importItems
    property var activeTransfer

    onImportItemsChanged: {
        if (importItems.length > 0) {
            dialogRoot.accepted()
        }
    }

    signal accepted()

    function open() {
        activeTransfer = picSourceMulti.request()
    }

    ContentPeer {
        id: picSourceMulti
        contentType: ContentType.Pictures |
                     ContentType.Documents
        handler: ContentHandler.Source
        selectionType: ContentTransfer.Multiple
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
