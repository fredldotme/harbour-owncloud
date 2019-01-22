import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.owncloud 1.0

Page {
    id: pageRoot

    readonly property bool loginInProgress : (authenticator.running ||
                                              browserCommandQueue.running);

    property NextcloudSettings clientSettings : null
    property OcsCommandQueue ocsCommandQueue : null
    property WebDavCommandQueue browserCommandQueue : null
    property DaemonControl daemonCtrl : null
    property AccountDb accountDatabase : null

    signal resetOcsInfo()
    signal notificationRequest(string summary, string body)

    Component.onCompleted: {
        if (clientSettings.autoLogin) {
            authenticator.authenticate(false)
        }
    }

    QWebDavAuthenticator {
        id: authenticator
        settings: clientSettings
        onAuthenticationSuccessful: {
            var addAccountSuccess = accountDatabase.addAccount(clientSettings)
            if (!addAccountSuccess) {
                console.warn("Failed to add account to database")
                return;
            }

            pageStack.pop()
            daemonCtrl.reloadConfig()
            browserCommandQueue.directoryListingRequest("/", false)
            browserCommandQueue.run()
            ocsCommandQueue.userInfoRequest();
            ocsCommandQueue.run()
        }
        onAuthenticationFailed: {
            console.log("onAuthenticationFailed")
            daemonCtrl.reloadConfig()
            notificationRequest(qsTr("Login failed"),
                                qsTr("Please check your connection or try again later."))
        }
        onSslError: {
            console.log("onSslError")
            ocsCommandQueue.stop()
            browserCommandQueue.stop()
            pageStack.completeAnimation();
            pageStack.clear();
            pageStack.replace(pageRoot)
            gc()
            pageStack.push(sslErrorDialogComponent,
                           {
                               md5Digest : certMd5,
                               sha1Digest : certSha1,
                               settings : settings
                           });
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: topIcon.height + mainColumn.height

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
            scale: 1.5
        }

        Column {
            id: mainColumn
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
                text: clientSettings.hoststring;
            }

            TextField {
                id: username
                enabled: !loginInProgress
                width: parent.width
                text: clientSettings.username;
                inputMethodHints: Qt.ImhNoAutoUppercase
                placeholderText: qsTr("User name")
                label: placeholderText
            }

            TextField {
                id: password
                enabled: !loginInProgress
                text: clientSettings.password
                width: parent.width
                echoMode: TextInput.Password
                placeholderText: qsTr("Password")
                label: placeholderText
            }

            ComboBox {
                width: parent.width
                enabled: !loginInProgress
                label: qsTr("Provider:")
                currentIndex: clientSettings.providerType
                onCurrentIndexChanged: clientSettings.providerType = currentIndex
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Nextcloud/ownCloud")
                    }
                    MenuItem {
                        text: qsTr("WebDav")
                    }
                }
            }

            TextSwitch {
                id: autoLoginSwitch
                enabled: !loginInProgress
                text: qsTr("Login automatically")
                checked: clientSettings.autoLogin
            }

            TextSwitch {
                id: certSwitch
                enabled: !loginInProgress
                text: qsTr("Accept certificate")
                visible: clientSettings.isCustomCert
                checked: clientSettings.isCustomCert
                onVisibleChanged: {
                    if (clientSettings && visible)
                        checked = clientSettings.isCustomCert
                }
            }

            Button {
                id: continueButton
                enabled: !loginInProgress
                text: qsTr("Continue")
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    resetOcsInfo()
                    if (clientSettings.setHostname(hostaddress.text)) {
                        clientSettings.username = username.text;
                        clientSettings.password = password.text;
                        clientSettings.autoLogin = autoLoginSwitch.checked;
                        clientSettings.isCustomCert = certSwitch.checked;

                        authenticator.authenticate(true)
                    } else {
                        notificationRequest(qsTr("Invalid URL"), qsTr("Please check your host address"))
                    }
                }
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: {
                    pageStack.push(aboutPageComponent)
                }
            }
        }

        AbortableBusyIndicator {
            anchors.centerIn: parent
            running: loginInProgress
            size: BusyIndicatorSize.Large
            // TODO: cancelable authentication
            /*onClicked: {
                authenticator.abortAuthentication();
            }*/
        }
    }
}
