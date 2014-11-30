import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: pageRoot
    anchors.fill: parent

    property bool loginFailed : false;
    property bool settingsMode : false;

    Component.onCompleted: {
        settings.readSettings();
        if(settings.autoLogin && !settingsMode) {
            loginInProgress = true;
            browser.testConnection();
        }
    }

    Connections {
        target: browser
        onLoginSucceeded: {
            loginInProgress = false;
            browser.getDirectoryContent("/");
            if(settingsMode) {
                pageStack.clear()
            }
            pageStack.replace("FileBrowser.qml");
        }
    }

    Connections {
        target: browser
        onLoginFailed: {
            loginInProgress = false;
        }
    }

    SilicaListView {
        anchors.fill: parent

        Label {
            id: topLabel
            text: "ownCloud"
            font.pixelSize: Theme.fontSizeLarge
            enabled: !loginInProgress
            x: (parent.width / 2) - (width / 2)
            anchors.horizontalCenter: pageRoot.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 20
        }

        NotificationPopup {
            visible: loginFailed
            height: topLabel.height + 20
        }

        TextField {
            id: hostaddress
            enabled: !loginInProgress
            width: parent.width
            anchors.top: topLabel.bottom
            anchors.topMargin: 20
            placeholderText: "Host address"
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
            placeholderText: "User name"
        }

        TextField {
            id: password
            enabled: !loginInProgress
            text: settings.password
            width: parent.width
            anchors.top: username.bottom
            echoMode: TextInput.Password
            placeholderText: "Password"
        }

        TextSwitch {
            id: autoLoginSwitch
            enabled: !loginInProgress
            anchors.top: password.bottom
            text: "Login automatically"
            checked: settings.autoLogin
        }

        TextSwitch {
            id: certSwitch
            enabled: !loginInProgress
            anchors.top: autoLoginSwitch.bottom
            text: "Accept certificate"
            visible: settings.isCustomCert
            checked: settings.isCustomCert
        }

        Button {
            id: continueButton
            enabled: !loginInProgress
            text: settingsMode ? "Save" : "Continue"
            anchors.top: certSwitch.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                settings.parseFromAddressString(hostaddress.text)

                settings.username = username.text;
                settings.password = password.text;
                settings.autoLogin = autoLoginSwitch.checked;
                settings.isCustomCert = certSwitch.checked;
                settings.writeSettings();

                loginInProgress = true;
                browser.testConnection();
            }
        }

        BusyIndicator {
            anchors.centerIn: parent
            running: loginInProgress
        }
    }


}
