import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Content 0.1

Item {
    id: dialogRoot
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
        }
    }

    signal accepted()

    function open() {
        var peer = null
        for (var i = 0; i < model.peers.length; ++i) {
            var p = model.peers[i]
            if (p.appId.indexOf("com.ubuntu.filemanager_") === 0) {
                peer = p
            }
        }

        activeTransfer = peer.request()
    }

    ContentPeerModel {
        id: model
        contentType: ContentType.Documents
        handler: ContentHandler.Source
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
