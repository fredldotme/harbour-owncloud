import QtQuick 2.0
import Sailfish.Silica 1.0
import OwnCloud 1.0

Page {
    id: pageRoot
    property string filePath : ""
    property EntryInfo entry;

        PageHeader {
            title: entry.name
        }

        Item {
            anchors.centerIn: pageRoot
            Label {
                text: "MIME type: " + entry.mimeType
            }
        }

}
