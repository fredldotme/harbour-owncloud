import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: pageRoot

    Component.onCompleted: {
        settings.readSettings();
    }

    RemorseItem {
        id: remorse
    }

    Label {
        id: topLabel
        text: "ownCloud"
        font.pixelSize: Theme.fontSizeLarge
        x: (parent.width / 2) - (width / 2)
        anchors.horizontalCenter: pageRoot
        anchors.topMargin: 20
    }

    TextField {
        id: hostaddress
        width: parent.width
        anchors.top: topLabel.bottom
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
        placeholderText: "User name"
    }

    TextField {
        id: password
        width: parent.width
        anchors.top: username.bottom
        echoMode: TextInput.Password
        placeholderText: "Password"
    }

    TextSwitch {
        id: certSwitch
        anchors.top: password.bottom
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
        anchors.horizontalCenter: pageRoot

        onClicked: {
            settings.parseFromAddressString(hostaddress.text)

            settings.username = username.text;
            settings.password = password.text;
            settings.writeSettings();

            browser.getDirectoryContent("/");
            pageStack.replace("FileBrowser.qml");
        }
    }
}
