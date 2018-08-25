import QtQuick 2.0
import Sailfish.Silica 1.0
import "qrc:/qml/pages/browser"

Item {
    Component.onCompleted: {
        pageStack.push(browserMain)
    }

    FileBrowser {
        id: browserMain
    }

}
