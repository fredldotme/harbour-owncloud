import QtQuick 2.0
import QtQuick.Dialogs 1.2

FileDialog {
    id: openFileDialog
    selectMultiple: true
    folder: !osIsIOS ?
                shortcuts.home :
                shortcuts.pictures
}
