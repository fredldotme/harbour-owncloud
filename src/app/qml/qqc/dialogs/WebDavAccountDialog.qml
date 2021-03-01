import QtQuick 2.0
import QtQuick.Controls 2.2
import harbour.owncloud 1.0
import "qrc:/qml-ui-set"

Dialog {
    id: dialogRoot
    standardButtons: Dialog.Cancel

    property var accountWorkers : null
    property var clientSettings: accountWorkers.account
    property DaemonControl daemonCtrl : null
    property AccountsDbInterface accountDatabase : null
    property var viewStack : null

    readonly property CommandQueue ocsCommandQueue :
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

            daemonCtrl.reloadConfig()
            clientSettings.resetSettings()
            dialogRoot.close()
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
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        parent: viewStack
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
        contentHeight: mainColumn.height
        clip: true
        ScrollBar.vertical: ScrollBar {}

        Column {
            id: mainColumn
            width: Math.min(parent.width, 400)
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
                placeholderText: qsTr("Username")
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

        AbortableBusyIndicator {
            anchors.centerIn: parent
            running: loginInProgress
            onAbort: authenticator.abort()
            buttonVisibiltyDelay: 1
        }
    }
}
