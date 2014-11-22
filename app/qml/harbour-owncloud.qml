import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow
{
    id: applicationWindow
    initialPage: Component { Login { id: loginPage } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
}


