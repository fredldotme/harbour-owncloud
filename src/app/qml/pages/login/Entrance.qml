import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Page {
    id: pageRoot
    anchors.fill: parent

    readonly property bool examining : examiner.running

    AuthenticationExaminer {
        id: examiner
        onExaminationSucceeded: {
            if (method === AuthenticationExaminer.FLOW_DIALOG) {
                pageStack.completeAnimation()
                pageStack.push(flowAuthenticationComponent);
            } else if (method === AuthenticationExaminer.QWEBDAV) {
                pageStack.completeAnimation()
                pageStack.push(basicAuthenticationComponent);
            }
        }

        onExaminationFailed: {
            // Use QWebdav for authentication support examination
            // if FLOW_DIALOG fails.
            console.log("Examination failed for method " + method)
            if (method === AuthenticationExaminer.FLOW_DIALOG) {
                pageStack.completeAnimation()
                pageStack.push(basicAuthenticationComponent);
            }
        }
    }

    Component.onCompleted: {
        // Prefer basic authentication for now
        // examiner.examine(persistentSettings.hoststring, AuthenticationExaminer.QWEBDAV)
    }

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
                    enabled: (serverUrl.text.indexOf("https://") == 0 ||
                              serverUrl.text.indexOf("http://") == 0)
                    onClicked: {
                        examiner.examine(serverUrl.text,
                                         AuthenticationExaminer.FLOW_DIALOG);
                    }
                }
            }
        }
    }
}
