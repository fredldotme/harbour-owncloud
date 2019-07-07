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
                title: qsTr("About GhostCloud")
            }

            Image {
                id: icon
                source: "qrc:/icons/icon_gray.svg"
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width / 2
                height: width
                scale: 1.5
                sourceSize.width: width
                sourceSize.height: height
            }

            Label {
                width: parent.width
                text: qsTr("GhostCloud")
                font.pixelSize: Theme.fontSizeLarge
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                width: parent.width
                text: qsTr("Easily access your Nextcloud, ownCloud and WebDav " +
                           "instances with a native Free Software application")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }
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
                            Qt.openUrlExternally("https://paypal.me/beidl")
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
                text: qsTr("Licensed under GPL v2")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }
            }

            Label {
                text: qsTr("Source code on GitHub")
                color: Theme.highlightColor
                font.underline: true
                font.pixelSize: Theme.fontSizeSmall
                horizontalAlignment: Text.AlignHCenter
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        Qt.openUrlExternally("https://github.com/fredldotme/harbour-owncloud")
                    }
                }
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }
            }

            Label {
                text: qsTr("libqtcommandqueue (LGPL v2.1)")
                font.pixelSize: Theme.fontSizeTiny
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }
            }
            Label {
                text: qsTr("QWebDAV library (qwebdavlib) version 1.0 (LGPL v2.1)")
                font.pixelSize: Theme.fontSizeTiny
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }
            }
            Label {
                text: qsTr("SailfishUiSet UI components (GPL v2)")
                font.pixelSize: Theme.fontSizeTiny
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }
            }
            Label {
                text: qsTr("Icon based on \"Cute clouds for CSS sprites\" by Yug (CC BY-SA 3.0)")
                font.pixelSize: Theme.fontSizeTiny
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }
            }
        }
    }
}
