import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: pageRoot
    anchors.fill: parent

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
        placeholderText: "Host address"
        text: "https://"
    }

    TextField {
        id: username
        width: parent.width
        anchors.top: hostaddress.bottom
        anchors.topMargin: 30
        text: settings.username != "" ? settings.username : "";
        placeholderText: "User name"
    }

    TextField {
        id: password
        width: parent.width
        anchors.top: username.bottom
        echoMode: TextInput.Password
        placeholderText: "Password"
    }

    Button {
        id: continueButton
        text: "Continue"
        anchors.top: password.bottom
        anchors.topMargin: 30
        width: parent.width
        anchors.horizontalCenter: pageRoot

        onClicked: {
            settings.parseFromAddressString(hostaddress.text)

            settings.username = username.text;
            settings.password = password.text;
            settings.writeSettings();

            var nextDirectory = Qt.createComponent("FileBrowser.qml");
            browser.getDirectoryContent("/");
            pageStack.push(nextDirectory)
        }
    }
}
