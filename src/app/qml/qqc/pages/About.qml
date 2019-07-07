import QtQuick 2.0
import QtQuick.Controls 2.2

Dialog {
    title: qsTr("About GhostCloud")
    focus: true

    readonly property bool destroyable : false
    standardButtons: Dialog.Ok

    signal closeRequest()

    Flickable {
        anchors.topMargin: fontSizeMedium
        anchors.fill: parent
        clip: true
        contentHeight: mainColumn.height + fontSizeLarge
        ScrollBar.vertical: ScrollBar {}

        Column {
            id: mainColumn
            width: parent.width
            spacing: fontSizeLarge

            Image {
                id: icon
                source: "qrc:/icons/icon_gray.svg"
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(parent.width / 3, 200)
                height: width
                scale: 1.5
                sourceSize.width: width
                sourceSize.height: height
            }

            Label {
                width: parent.width
                text: qsTr("GhostCloud")
                font.pixelSize: fontSizeLarge
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                width: parent.width
                text: qsTr("Easily access your Nextcloud, ownCloud and WebDav instances " +
                           "with a native, multi-platform Free Software application")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: fontSizeLarge
                    rightMargin: fontSizeLarge
                }
            }

            MenuSeparator {
                width: parent.width
            }
            Label {
                text: qsTr("Donation")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 16
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: fontSizeLarge

                Label {
                    text: qsTr("Via PayPal")
                    font.underline: true
                    font.pixelSize: fontSizeSmall
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            Qt.openUrlExternally("https://paypal.me/beidl")
                        }
                    }
                }

                Label {
                    text: qsTr("Via Flattr")
                    font.underline: true
                    font.pixelSize: fontSizeSmall
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            Qt.openUrlExternally("https://flattr.com/@beidl")
                        }
                    }
                }
            }

            MenuSeparator {
                width: parent.width
            }
            Label {
                text: qsTr("Copyright notices")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 16
            }

            Label {
                text: qsTr("Licensed under GPL v2")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: fontSizeLarge
                    rightMargin: fontSizeLarge
                }
            }

            Label {
                text: qsTr("Source code on GitHub")
                font.underline: true
                font.pixelSize: fontSizeSmall
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
                    leftMargin: fontSizeLarge
                    rightMargin: fontSizeLarge
                }
            }

            Label {
                text: qsTr("libqtcommandqueue (LGPL v2.1)")
                font.pixelSize: fontSizeTiny
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: fontSizeLarge
                    rightMargin: fontSizeLarge
                }
            }
            Label {
                text: qsTr("QWebDAV library (qwebdavlib) version 1.0 (LGPL v2.1)")
                font.pixelSize: fontSizeTiny
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: fontSizeLarge
                    rightMargin: fontSizeLarge
                }
            }
            Label {
                text: qsTr("QmlUiSet UI components (LGPL v2.1)")
                font.pixelSize: fontSizeTiny
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: fontSizeLarge
                    rightMargin: fontSizeLarge
                }
            }
            Label {
                text: qsTr("KDE Breeze Icon Theme (LGPL v2.1)")
                font.pixelSize: fontSizeTiny
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: fontSizeLarge
                    rightMargin: fontSizeLarge
                }
            }
            Label {
                text: qsTr("Icon based on \"Cute clouds for CSS sprites\" by Yug (CC BY-SA 3.0)")
                font.pixelSize: fontSizeTiny
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: fontSizeLarge
                    rightMargin: fontSizeLarge
                }
            }
        }
    }
}
