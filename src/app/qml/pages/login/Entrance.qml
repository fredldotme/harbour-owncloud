import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Page {
    id: pageRoot
    anchors.fill: parent

    readonly property Component basicAuthenticationComponent :
        Qt.createComponent("qrc:/qml/pages/login/BasicAuthentication.qml");
    readonly property Component flowAuthenticationComponent :
        Qt.createComponent("qrc:/qml/pages/login/FlowAuthentication.qml");
    property bool examining : false

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("Manual configuration")
                onClicked: pageStack.push(basicAuthenticationComponent);
            }
        }

        Image {
            id: topIcon
            source: "qrc:/icons/icon_gray.svg"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 48
            width: parent.width / 3
            height: width
            sourceSize.width: width
            sourceSize.height: height
            scale: 2
        }

        Item {
            anchors.centerIn: parent
            width: parent.width
            visible: examining

            BusyIndicator {
                anchors.centerIn: parent
                running: examining
                size: BusyIndicatorSize.Large
            }
        }

        Item {
            anchors.centerIn: parent
            width: parent.width
            visible: !examining

            Column {
                anchors.fill: parent
                spacing: 32

                AuthenticationExaminer {
                    id: examiner
                    onExaminationSucceeded: {
                        examining = false
                        if (method === AuthenticationExaminer.FLOW_DIALOG) {
                            pageStack.push(flowAuthenticationComponent);
                        } else if (method === AuthenticationExaminer.QWEBDAV) {
                            pageStack.push(basicAuthenticationComponent);
                        }
                    }

                    onExaminationFailed: {
                        examining = false

                        // Use QWebdav for authentication support examination
                        // if FLOW_DIALOG fails.
                        if (method === AuthenticationExaminer.FLOW_DIALOG) {
                            pageStack.push(basicAuthenticationComponent);
                        }
                    }
                }

                TextField {
                    id: serverUrl
                    placeholderText: qsTr("Host address")
                    label: placeholderText
                    text: "https://"
                    width: parent.width
                    focus: !examining
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Continue")
                    enabled: !(serverUrl.text === "https://" ||
                               serverUrl.text === "http://")
                    onClicked: {
                        examining = true
                        examiner.examine(serverUrl.text,
                                         AuthenticationExaminer.FLOW_DIALOG);
                    }
                }
            }
        }
    }
}
