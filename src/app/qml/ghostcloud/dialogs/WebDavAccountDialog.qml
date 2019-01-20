import QtQuick 2.0
import QtQuick.Controls 2.2
import harbour.owncloud 1.0

Rectangle {
    id: dialogRoot
    color: "white"

    property var accountWorkers : null
    property var clientSettings: accountWorkers.account
    property DaemonControl daemonCtrl : null
    property AccountDb accountDatabase : null
    property var viewStack : null

    readonly property OcsCommandQueue ocsCommandQueue :
        accountWorkers.accountInfoCommandQueue
    readonly property CloudStorageProvider browserCommandQueue :
        accountWorkers.browserCommandQueue

    signal resetOcsInfo()
    signal notificationRequest(string summary, string body)

    property bool customTlsCertRequired : false

    readonly property bool loginInProgress :
        (authenticator.running ||
         browserCommandQueue.running);


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

            viewStack.pop()
            daemonCtrl.reloadConfig()
            /*browserCommandQueue.directoryListingRequest("/", false)
            browserCommandQueue.run()
            ocsCommandQueue.userInfoRequest();
            ocsCommandQueue.run()*/
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
            gc()
            customTlsCertRequired = true
            sslErrorDialog.md5Fingerprint = certMd5
            sslErrorDialog.sha1Fingerprint = certSha1
            sslErrorDialog.open()
        }
    }

    SSLErrorDialog {
        id: sslErrorDialog
        onAccepted: {
            console.log("Accepting certificate...")
            clientSettings.acceptTlsFingerprints(sslErrorDialog.md5Fingerprint,
                                                 sslErrorDialog.sha1Fingerprint)
            certSwitch.checked = true
            sslErrorDialog.md5Fingerprint = ""
            sslErrorDialog.sha1Fingerprint = ""
        }
        onRejected: {
            customTlsCertRequired = false
        }
    }

    Flickable {
        anchors.fill: parent
        contentHeight: topIcon.height + mainColumn.height

        Image {
            id: topIcon
            source: "qrc:/icons/icon_gray.svg"
            enabled: !loginInProgress
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 48
            width: Math.min(parent.width / 3, 600)
            height: Math.min(width, 800)
            sourceSize.width: width
            sourceSize.height: height
            scale: 1.5
        }

        Column {
            id: mainColumn
            width: Math.min(parent.width, 400)
            anchors.top: topIcon.bottom
            anchors.topMargin: 48
            anchors.horizontalCenter: parent.horizontalCenter

            TextField {
                id: hostaddress
                enabled: !loginInProgress
                width: parent.width
                placeholderText: qsTr("Host address")
                text: clientSettings.hoststring;
            }

            TextField {
                id: username
                enabled: !loginInProgress
                width: parent.width
                text: clientSettings.username;
                inputMethodHints: Qt.ImhNoAutoUppercase
                placeholderText: qsTr("User name")
            }

            TextField {
                id: password
                enabled: !loginInProgress
                text: clientSettings.password
                width: parent.width
                echoMode: TextInput.Password
                placeholderText: qsTr("Password")
            }

            ComboBox {
                width: parent.width
                enabled: !loginInProgress
                displayText: qsTr("Provider:") + currentText
                currentIndex: clientSettings.providerType
                onCurrentIndexChanged: {
                    console.log(clientSettings)
                    console.log(clientSettings
                                ? clientSettings.providerType
                                : "")
                    clientSettings.providerType = currentIndex
                }
                model: [ qsTr("Nextcloud/ownCloud"), qsTr("WebDav") ]
            }

            CheckBox {
                id: certSwitch
                enabled: !loginInProgress
                text: qsTr("Accept certificate")
                visible: customTlsCertRequired
                checked: clientSettings.isCustomCert
                onCheckedChanged: {
                    if (!checked)
                        clientSettings.isCustomCert = false
                }
            }

            Button {
                id: continueButton
                enabled: !loginInProgress
                text: qsTr("Continue")
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    resetOcsInfo()
                    if (clientSettings.setHoststring(hostaddress.text)) {
                        clientSettings.username = username.text;
                        clientSettings.password = password.text;
                        clientSettings.isCustomCert = certSwitch.checked;

                        authenticator.authenticate(true)
                    } else {
                        notificationRequest(qsTr("Invalid URL"), qsTr("Please check your host address"))
                    }
                }
            }
        }

        BusyIndicator {
            anchors.centerIn: parent
            running: loginInProgress
        }
    }
}
