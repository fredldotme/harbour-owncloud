import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    anchors.fill: parent

    SilicaFlickable {
        anchors.fill: parent
        clip: true
        contentHeight: mainColumn.height + Theme.paddingLarge

        Column {
            id: mainColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                id: header
                title: qsTr("About Nextcloud/ownCloud client")
            }

            Image {
                id: icon
                source: "qrc:/icons/icon_gray.svg"
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width / 2
                height: width
                scale: 2
                sourceSize.width: width
                sourceSize.height: height
            }

            Label {
                width: parent.width
                text: qsTr("Nextcloud/ownCloud client")
                font.pixelSize: Theme.fontSizeLarge
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                width: parent.width
                text: qsTr("Easily access your ownCloud and NextCloud " +
                           "instances with a native free software application.")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
            }

            Separator {
                width: parent.width
                color: Theme.secondaryColor
                horizontalAlignment: Qt.AlignHCenter
            }
            SectionHeader {
                text: qsTr("Donation")
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingLarge

                Label {
                    text: qsTr("Via PayPal")
                    color: Theme.highlightColor
                    font.underline: true
                    font.pixelSize: Theme.fontSizeSmall
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            Qt.openUrlExternally("https://www.paypal.com/cgi-bin/webscr?cmd=_donations"+
                                                 "&business=dev.beidl@gmail.com&lc=US&item_name="+
                                                 "Nextcloud+ownCloud+client&no_note=0&no_shipping=2&"+
                                                 "curency_code=EUR")
                        }
                    }
                }

                Label {
                    text: qsTr("Via Flattr")
                    color: Theme.highlightColor
                    font.underline: true
                    font.pixelSize: Theme.fontSizeSmall
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            Qt.openUrlExternally("https://flattr.com/@beidl")
                        }
                    }
                }
            }

            Separator {
                width: parent.width
                color: Theme.secondaryColor
                horizontalAlignment: Qt.AlignHCenter
            }
            SectionHeader {
                text: qsTr("Copyright notices")
            }

            Label {
                width: parent.width
                text: qsTr("Licensed under GPL v2")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                text: qsTr("Source code on GitHub")
                color: Theme.highlightColor
                font.underline: true
                font.pixelSize: Theme.fontSizeSmall
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        Qt.openUrlExternally("https://github.com/beidl/harbour-owncloud")
                    }
                }
            }

            Label {
                width: parent.width
                text: qsTr("QWebDAV library (qwebdavlib) version 1.0 (LGPL v2.1)")
                font.pixelSize: Theme.fontSizeTiny
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
            }
            Label {
                width: parent.width
                text: qsTr("SailfishUiSet UI components (GPL v2)")
                font.pixelSize: Theme.fontSizeTiny
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
