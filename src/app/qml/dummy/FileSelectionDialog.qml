// This file exists only to translate
// qml-ui-set/ui/FileSelectionDialog.qml
// For some reason the files are not included
// for translation when generating the package...

import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    ComboBox {
        id: storageSelection
        label: qsTr("Storage:")
        menu: ContextMenu {
            MenuItem {
                id: userDirMenuItem
                text: qsTr("Home directory")
            }
            MenuItem {
                id: sdCardMenuItem
                text: qsTr("External storage")
            }
            MenuItem {
                id: usbOtgMenuItem
                text: qsTr("External storage (2)")
            }
        }
    }
}
