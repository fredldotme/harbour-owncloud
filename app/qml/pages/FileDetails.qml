import QtQuick 2.0
import Sailfish.Silica 1.0
import OwnCloud 1.0

Page {
    id: pageRoot
    property EntryInfo entry;

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                id: download
                text: qsTr("Download")
                enabled: downloader.isNotEnqueued(entry);
                onClicked: {
                    var download = downloader.enqueueDownload(entry, false)
                    enabled = false;
                }
            }

            MenuItem {
                id: downloadAndOpen
                text: qsTr("Download and open")
                enabled: downloader.isNotEnqueued(entry);
                onClicked: {
                    var download = downloader.enqueueDownload(entry, true)
                    enabled = false;
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
            id: remotePathLabel
            text: "Remote path: " + entry.path
            anchors.top: fileImage.bottom
            anchors.topMargin: 32
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: sizeLabel
            text: "Size: " + entry.size + " bytes"
            anchors.top: remotePathLabel.bottom
            anchors.topMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            text: "MIME type: " + entry.mimeType
            visible: entry.mimeType != ""
            anchors.top: sizeLabel.bottom
            anchors.topMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
