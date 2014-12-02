import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: pageRoot

    Component.onCompleted: {
        listView.model = transfer.getTransfers();
    }

    Connections {
        target: transfer
        onTransferAdded: {
            listView.model = transfer.getTransfers();
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        SilicaListView {
            id: listView
            anchors.fill: parent
            header: PageHeader {
                title: "File transfers"
            }

            delegate: BackgroundItem {
                id: delegate

                Image {
                    id: transferTypeIcon
                    x: 16
                    height: label.height + transferProgress.height
                    source: listView.model[index].direction === 0 ?
                                "image://theme/icon-m-download" :
                                "image://theme/icon-m-upload"
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: label
                    text: listView.model[index].name
                    anchors.left: transferTypeIcon.right
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                }

                ProgressBar {
                    id: transferProgress
                    anchors.top: label.bottom
                    width: parent.width
                    anchors.leftMargin: Theme.paddingLarge
                }

                Connections {
                    /*
                      Use a connection instead of directly assigning model[index].value
                      The TransferEntry object appears to get deleted between emitting the
                      signal and trying to read the property. Use this connection instead.
                      */
                    target: listView.model[index]
                    onProgressChanged: {
                        transferProgress.value = progress
                    }
                }
            }
            VerticalScrollDecorator {}

            Label {
                id: noTransfersHint
                anchors.centerIn: parent
                text: qsTr("No file transfers pending")
                visible: listView.model === undefined || listView.model.length === 0
            }
        }
    }
}
