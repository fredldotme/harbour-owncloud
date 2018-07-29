import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Page {
    id: pageRoot

    readonly property bool loginInProgress : authenticator.running;

    Component.onCompleted: {
        persistentSettings.readSettings();
        if (persistentSettings.autoLogin) {
            authenticator.authenticate(false)
        }
    }

    QWebDavAuthenticator {
        id: authenticator
        settings: persistentSettings
        onAuthenticationSuccessful: {
            daemonCtrl.reloadConfig()
            pageStack.completeAnimation()
            pageStack.clear()
            pageStack.push(browserComponent, {}, PageStackAction.Immediate);
            ocsCommandQueue.userInfoRequest();
            ocsCommandQueue.run()
        }
        onAuthenticationFailed: {
            daemonCtrl.reloadConfig()
        }
        onSslError: {
            pageStack.completeAnimation();
            pageStack.push(sslErrorDialogComponent,
                           {
                               md5Digest : md5Digest,
                               sha1Digest : sha1Digest,
                               settings : persistentSettings
                           });
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        Image {
            id: topIcon
            source: "qrc:/icons/icon_gray.svg"
            enabled: !loginInProgress
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 48
            width: parent.width / 3
            height: width
            sourceSize.width: width
            sourceSize.height: height
            scale: 2
        }

        Column {
            width: parent.width
            anchors.top: topIcon.bottom
            anchors.topMargin: 48
            anchors.horizontalCenter: parent.horizontalCenter

            TextField {
                id: hostaddress
                enabled: !loginInProgress
                width: parent.width
                placeholderText: qsTr("Host address")
                label: placeholderText
                text: persistentSettings.hoststring;
            }

            TextField {
                id: username
                enabled: !loginInProgress
                width: parent.width
                text: persistentSettings.username;
                inputMethodHints: Qt.ImhNoAutoUppercase
                placeholderText: qsTr("User name")
                label: placeholderText
            }

            TextField {
                id: password
                enabled: !loginInProgress
                text: persistentSettings.password
                width: parent.width
                echoMode: TextInput.Password
                placeholderText: qsTr("Password")
                label: placeholderText
            }

            TextSwitch {
                id: autoLoginSwitch
                enabled: !loginInProgress
                text: qsTr("Login automatically")
                checked: persistentSettings.autoLogin
            }

            TextSwitch {
                id: certSwitch
                enabled: !loginInProgress
                text: qsTr("Accept certificate")
                visible: persistentSettings.isCustomCert
                checked: persistentSettings.isCustomCert
            }

            Button {
                id: continueButton
                enabled: !loginInProgress
                text: qsTr("Continue")
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    if (persistentSettings.parseFromAddressString(hostaddress.text)) {
                        persistentSettings.username = username.text;
                        persistentSettings.password = password.text;
                        persistentSettings.autoLogin = autoLoginSwitch.checked;
                        persistentSettings.isCustomCert = certSwitch.checked;
                        persistentSettings.writeSettings();
                        persistentSettings.readSettings()

                        authenticator.authenticate(true)
                    } else {
                        notify(qsTr("Invalid URL"), qsTr("Please check your host address"))
                    }
                }
            }
        }

        BusyIndicator {
            anchors.centerIn: parent
            running: loginInProgress
            size: BusyIndicatorSize.Large
        }
    }
}
