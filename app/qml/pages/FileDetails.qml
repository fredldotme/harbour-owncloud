import QtQuick 2.0
import Sailfish.Silica 1.0
import OwnCloud 1.0

Page {
    id: pageRoot
    property string filePath : ""
    property EntryInfo entry;

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                id: download
                text: qsTr("Download")
                enabled: downloader.isNotEnqueued(entry);
                onClicked: {
                    var download = downloader.enqueueDownload(entry)
                    enabled = false;
                }
            }
        }

        PageHeader {
            title: entry.name
        }

        Label {
            text: "MIME type: " + entry.mimeType
        }
    }
}
