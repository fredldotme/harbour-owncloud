import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Page {
    id: pageRoot
    property EntryInfo entry;
    property TransferEntry downloadEntry;

    function getHRSize(bytes) {
        var units = ['bytes', 'kB', 'MB', 'GB', 'TB'];
        var i = 0;

        for(; i < 5 && bytes >= 1024; i++) {
            bytes = bytes / 1024;
        }

        return bytes.toFixed(2) + " " + units[i];
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                id: download
                text: qsTr("Download")
                enabled: transfer.isNotEnqueued(entry);
                onClicked: {
                    downloadEntry = transfer.enqueueDownload(entry, false)
                    enabled = false;
                }

                Connections {
                    target: downloadEntry
                    onTransferCompleted: {
                        download.enabled = true
                    }
                }
            }

            MenuItem {
                id: downloadAndOpen
                text: qsTr("Download and open")
                enabled: transfer.isNotEnqueued(entry);
                onClicked: {
                    downloadEntry = transfer.enqueueDownload(entry, true)
                    enabled = false;
                }

                Connections {
                    target: downloadEntry
                    onTransferCompleted: {
                        downloadAndOpen.enabled = true
                    }
                }
            }
        }

        PageHeader {
            title: entry.name
        }

        Image {
            id: fileImage
            source: "../images/large-file.png"
            width: parent.width / 3
            height: width
            x: (parent.width / 2) - (width / 2)
            y: width
        }

        Label {
            x: 0
            anchors.top: fileImage.bottom
            anchors.topMargin: 32
            text: "Size:"
            width: (parent.width / 2) - 12
            horizontalAlignment: Text.AlignRight
            color: Theme.highlightColor
        }

        Label {
            id: sizeHint
            x: (parent.width / 2) + 24
            anchors.top: fileImage.bottom
            anchors.topMargin: 32
            text: getHRSize(entry.size)
            width: (parent.width / 2) - 12
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.Wrap
        }

        Label {
            x: 0
            y: sizeHint.y + sizeHint.height
            visible: entry.mimeType != ""
            text: "Type:"
            width: (parent.width / 2) - 12
            horizontalAlignment: Text.AlignRight
            color: Theme.highlightColor
        }

        Label {
            id: typeHint
            x: (parent.width / 2) + 24
            y: sizeHint.y + sizeHint.height
            width: (parent.width / 2) - 12
            visible: entry.mimeType != ""
            text: entry.mimeType
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.Wrap
        }
    }
}
