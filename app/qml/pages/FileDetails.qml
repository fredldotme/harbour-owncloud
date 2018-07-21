import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0
import SailfishUiSet 1.0

Page {
    id: pageRoot
    property RemoteFileInfo entry;
    property CommandEntity downloadEntry;

    Connections {
        target: downloadEntry
        onDone: {
            download.enabled = true
            downloadAndOpen.enabled = true
        }
    }

    FileDetailsHelper { id: fileDetailsHelper }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                id: download
                text: qsTr("Download")
                enabled: transfer.isNotEnqueued(entry.path);
                onClicked: {
                    downloadEntry = transfer.enqueueDownload(entry.path, entry.mimeType, false)
                    download.enabled = false
                    downloadAndOpen.enabled = false
                }
            }

            MenuItem {
                id: downloadAndOpen
                text: qsTr("Download and open")
                enabled: transfer.isNotEnqueued(entry.path);
                onClicked: {
                    downloadEntry = transfer.enqueueDownload(entry.path, entry.mimeType, true)
                    download.enabled = false
                    downloadAndOpen.enabled = false
                }
            }
        }

        PageHeader {
            id: header
            title: qsTr("Details")
        }

        ThumbnailFetcher {
            id: thumbnailFetcher
            settings: browser.settings
            commandQueue: transfer.mainQueue
            width: fileImage.width
            height: fileImage.height
            Component.onCompleted: {
                settings.readSettings()
                thumbnailFetcher.fetchThumbnail(entry.path);
            }
        }

        // Icon & Progress spinner
        Item {
            property int margins : parent.width / 6

            id: fileImage
            width: parent.width / 3
            height: width
            anchors.top: header.bottom
            anchors.topMargin: margins
            anchors.left: parent.left
            anchors.leftMargin: margins
            anchors.right: parent.right
            anchors.rightMargin: margins


            Image {
                source: thumbnailFetcher.source
                anchors.fill: parent
            }
            BusyIndicator {
                id: progressSpinner
                size: BusyIndicatorSize.Large
                anchors.fill: parent
                running: thumbnailFetcher.fetching
                visible: running && (thumbnailFetcher.source === "")
                enabled: visible
            }
            ViewPlaceholder {
                anchors.fill: parent
                enabled: !progressSpinner.enabled && thumbnailFetcher.source
                text: qsTr("Loading thumbnail")
            }
        }

        // File name
        Label {
            x: 0
            anchors.top: fileImage.bottom
            anchors.topMargin: 32
            text: qsTr("File name:")
            width: (parent.width / 2) - 12
            horizontalAlignment: Text.AlignRight
            color: Theme.highlightColor
        }

        Label {
            id: nameHint
            x: (parent.width / 2) + 24
            anchors.top: fileImage.bottom
            anchors.topMargin: 32
            text: entry.name
            width: (parent.width / 2) - 12
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.Wrap
        }

        // Size
        Label {
            x: 0
            y: nameHint.y + nameHint.height
            text: qsTr("Size:")
            width: (parent.width / 2) - 12
            horizontalAlignment: Text.AlignRight
            color: Theme.highlightColor
        }

        Label {
            id: sizeHint
            x: (parent.width / 2) + 24
            y: nameHint.y + nameHint.height
            width: (parent.width / 2) - 12
            text: fileDetailsHelper.getHRSize(entry.size)
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.Wrap
        }

        // Last modified
        Label {
            x: 0
            y: sizeHint.y + sizeHint.height
            text: qsTr("Last modified:")
            width: (parent.width / 2) - 12
            horizontalAlignment: Text.AlignRight
            color: Theme.highlightColor
        }

        Label {
            id: modHint
            x: (parent.width / 2) + 24
            y: sizeHint.y + sizeHint.height
            width: (parent.width / 2) - 12
            text: Qt.formatDateTime(entry.modTime, Qt.SystemLocaleShortDate);
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.Wrap
        }

        // Type
        Label {
            x: 0
            y: modHint.y + modHint.height
            visible: entry.mimeType != ""
            text: qsTr("Type:")
            width: (parent.width / 2) - 12
            horizontalAlignment: Text.AlignRight
            color: Theme.highlightColor
        }

        Label {
            id: typeHint
            x: (parent.width / 2) + 24
            y: modHint.y + modHint.height
            width: (parent.width / 2) - 12
            visible: entry.mimeType != ""
            text: entry.mimeType
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.Wrap
        }
    }
}
