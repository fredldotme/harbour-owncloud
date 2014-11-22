import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    property string md5Digest;
    property string sha1Digest;

    Label {
        id: md5Label
        text: md5Digest
    }

    Label {
        id: sha1Label
        text: sha1Digest
    }
}
