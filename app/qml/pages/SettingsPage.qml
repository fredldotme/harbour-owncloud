import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: pageRoot

    property bool loginFailed : false;
    property bool loginInProgress : false;

    Component.onCompleted: {
        settings.readSettings();
    }

    TextField {
        id: hostaddress
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: 20
        placeholderText: "Host address"
        text: settings.hoststring;
    }

    TextField {
        id: username
        width: parent.width
        anchors.top: hostaddress.bottom
        anchors.topMargin: 30
        text: settings.username;
        inputMethodHints: Qt.ImhNoAutoUppercase
        placeholderText: "User name"
    }

    TextField {
        id: password
        text: settings.password
        width: parent.width
        anchors.top: username.bottom
        echoMode: TextInput.Password
        placeholderText: "Password"
    }

    TextSwitch {
        id: autoLoginSwitch
        anchors.top: password.bottom
        text: "Login automatically"
        checked: settings.autoLogin
    }

    TextSwitch {
        id: certSwitch
        anchors.top: autoLoginSwitch.bottom
        text: "Accept certificate"
        visible: settings.isCustomCert
        checked: settings.isCustomCert
    }

    Button {
        id: continueButton
        text: "Continue"
        anchors.top: certSwitch.bottom
        anchors.topMargin: 30
        width: parent.width
        anchors.horizontalCenter: pageRoot.horizontalCenter

        onClicked: {
            settings.parseFromAddressString(hostaddress.text)

            settings.username = username.text;
            settings.password = password.text;
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
