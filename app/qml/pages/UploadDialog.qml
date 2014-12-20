import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Dialog {
    id: fileUploadDialog
    canAccept: fileToUpload != "";

    property string fileToUpload;
    property string remotePath;

    LocalFileBrowser {
        id: fileBrowser
    }

    onAccepted: {
        transfer.enqueueUpload(fileToUpload, browser.getCurrentPath());
    }

    NumberAnimation {
        id: outAnimation
        target: listView
        property: "opacity"
        duration: 100
        easing.type: Easing.InOutQuad
        from: 1.0
        to: 0.0
        running: false

        onStopped: {
            listView.model = fileBrowser.cd(fileBrowser.path);
            inAnimation.start()
        }
    }

    NumberAnimation {
        id: inAnimation
        target: listView
        property: "opacity"
        duration: 100
        easing.type: Easing.OutInQuad
        from: 0.0
        to: 1.0
        running: false

        onStopped: {
            listView.enabled = true;
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        DialogHeader {
            id: header
            acceptText: "Upload"
            cancelText: "Cancel"
        }

        SilicaListView {
            id: listView
            y: header.y + header.height
            width: parent.width
            height: parent.height - y
            model: fileBrowser.cd()

            delegate: BackgroundItem {
                id: delegate

                Rectangle {
                    color: Theme.highlightColor
                    visible: listView.model[index].path === fileToUpload;
                    anchors.fill: parent
                    opacity: 0.5
                }

                Image {
                    id: icon
                    source: listView.model[index].isDirectory ?
                                (listView.model[index].name !== ".." ?
                                    "image://theme/icon-m-folder" :
                                    "image://theme/icon-m-back"
                                 ) :
                                getIconFromMime(listView.model[index].mimeType)
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.top: parent.top
                    anchors.topMargin: 18
                    height: label.height
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: label
                    x: icon.x + icon.width + 6
                    y: icon.y - icon.height + 6
                    visible: listView.model[index].name !== ".."
                    text: listView.model[index].name
                    anchors.verticalCenter: parent.verticalCenter
                    color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor

                }

                onClicked: {
                    if(listView.model[index].isDirectory) {
                        fileBrowser.path = listView.model[index].path;
                        listView.enabled = false;
                        outAnimation.start()
                    } else {
                        if(listView.model[index].path === fileToUpload) {
                            fileToUpload = "";
                        } else {
                            fileToUpload = listView.model[index].path;
                        }
                    }
                }
            }
        }
    }
}
