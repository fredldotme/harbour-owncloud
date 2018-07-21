import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: pageRoot

    Component.onCompleted: {
        settings.readSettings();
        if(settings.autoLogin && !loginFailed) {
            loginInProgress = true;
            browser.testConnection();
        }
    }

    Connections {
        target: browser
        onLoginSucceeded: {
            loginInProgress = false;
            pageStack.replace("qrc:/qml/pages/FileBrowser.qml");
            daemonCtrl.reloadConfig()

            var command = browser.directoryListingRequest("/");
            transfer.mainQueue.enqueue(command)
            transfer.mainQueue.run()
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        Label {
            id: topLabel
            text: "ownCloud"
            font.pixelSize: Theme.fontSizeLarge
            enabled: !loginInProgress
            x: (parent.width / 2) - (width / 2)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 48
            anchors.bottomMargin: 48
        }

        TextField {
            id: hostaddress
            enabled: !loginInProgress
            width: parent.width
            anchors.top: topLabel.bottom
            anchors.topMargin: 20
            placeholderText: qsTr("Host address")
            label: placeholderText
            text: settings.hoststring;
        }

        TextField {
            id: username
            enabled: !loginInProgress
            width: parent.width
            anchors.top: hostaddress.bottom
            anchors.topMargin: 30
            text: settings.username;
            inputMethodHints: Qt.ImhNoAutoUppercase
            placeholderText: qsTr("User name")
            label: placeholderText
        }

        TextField {
            id: password
            enabled: !loginInProgress
            text: settings.password
            width: parent.width
            anchors.top: username.bottom
            echoMode: TextInput.Password
            placeholderText: qsTr("Password")
            label: placeholderText
        }

        TextSwitch {
            id: autoLoginSwitch
            enabled: !loginInProgress
            anchors.top: password.bottom
            text: qsTr("Login automatically")
            checked: settings.autoLogin
        }

        TextSwitch {
            id: certSwitch
            enabled: !loginInProgress
            anchors.top: autoLoginSwitch.bottom
            text: qsTr("Accept certificate")
            visible: settings.isCustomCert
            checked: settings.isCustomCert
        }

        Button {
            id: continueButton
            enabled: !loginInProgress
            text: qsTr("Continue")
            anchors.top: certSwitch.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                if(settings.parseFromAddressString(hostaddress.text)) {
                    settings.username = username.text;
                    settings.password = password.text;
                    settings.autoLogin = autoLoginSwitch.checked;
                    settings.isCustomCert = certSwitch.checked;
                    settings.writeSettings();
                    settings.readSettings()

                    loginInProgress = true;
                    loginFailed = false;
                    browser.testConnection();
                } else {
                    notify(qsTr("Invalid URL"), qsTr("Please check your host address"))
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
