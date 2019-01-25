import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtMultimedia 5.6
import harbour.owncloud 1.0
import QmlUiSet 1.0
import "qrc:/qml/qqc"
import "qrc:/qml/qqc/controls"

Page {
    id: pageRoot
    
    FileDetailsHelper { id: fileDetailsHelper}

    property var entry : null;
    property AccountWorkers accountWorkers : null
    property ThumbnailFetcher thumbnailFetcher : accountWorkers.thumbnailFetcher

    readonly property string imgSrc :
        (!entry.isDirectory && !thumbnailFetcher.fetching &&
         thumbnailFetcher.source !== "") ?
            thumbnailFetcher.source :
            fileDetailsHelper.getIconFromMime(entry.mimeType)

    readonly property bool isAudioVideo :
        (!entry.isDirectory &&
         (entry.mimeType.indexOf("video") === 0 ||
          entry.mimeType.indexOf("audio") === 0))

    readonly property string fileExistsHintText :
        qsTr("The file '%1' already exists. Would you like to overwrite it?")

    Component.onCompleted: {
        if (isAudioVideo || entry.isDirectory)
            return;

        thumbnailFetcher.remoteFile = entry.path
        thumbnailFetcher.width = 256
        thumbnailFetcher.height = 256
        thumbnailFetcher.fetch();
        console.log("Fetching thumbnail: " + thumbnailFetcher.remoteFile + " @ " + entry.path)
    }
    
    Flickable {
        contentHeight: mainColumn.height + (paddingLarge*8)
        anchors.fill: parent

        Column {
            id: mainColumn
            anchors.fill: parent
            anchors.topMargin: paddingLarge
            spacing: paddingMedium

            Item {
                // Wider side margins when only showing
                // a default icon from the system theme
                property int margins : (thumbnailFetcher.source === "" && !isAudioVideo)
                                       ? (parent.width/4)
                                       : (parent.width/8)
                id: filePreview
                width: parent.width / 3
                height: width
                anchors.horizontalCenter: parent.horizontalCenter

                // Image thumbnail
                Button {
                    id: thumbnailView
                    anchors.fill: parent
                    background: Rectangle { color: "transparent" }
                    icon.color: "transparent"
                    icon.name: (entry.isDirectory ? "folder" : imgSrc)
                    visible: !isAudioVideo
                }

                Image {
                    id: thumbnail
                    anchors.fill: parent
                    source: imgSrc
                    visible: !isAudioVideo
                }

                // Media player for video and audio preview
                WebDavMediaFeeder {
                    id: mediaFeeder
                    mediaPlayer: previewPlayer
                    settings: accountWorkers.account
                    url: isAudioVideo ?
                             (FilePathUtil.getWebDavFileUrl(entry.path, accountWorkers.account)) :
                             ""
                }
                MediaPlayer {
                    id: previewPlayer
                    autoPlay: false
                    onSourceChanged: {
                        console.log("AV preview " + source)
                    }
                    onPlaybackStateChanged: {
                        console.log("playback state " + playbackState)
                    }
                }
                VideoOutput {
                    id: mediaView
                    source: previewPlayer
                    visible: isAudioVideo
                    anchors.fill: parent
                }
                Button {
                    anchors.centerIn: parent
                    background: Rectangle { color: "transparent" }
                    icon.color: "transparent"
                    icon.name:
                        (previewPlayer.playbackState == MediaPlayer.PlayingState) ?
                            "media-playback-pause" :
                            "media-playback-start"
                    visible: mediaView.visible
                    onClicked: {
                        console.log("clicked @ " + previewPlayer.playbackState)
                        if (previewPlayer.playbackState == MediaPlayer.PlayingState) {
                            mediaFeeder.pause()
                        } else {
                            mediaFeeder.play()
                        }
                    }
                }

                // Loading indicator
                BusyIndicator {
                    id: progressSpinner
                    anchors.centerIn: parent
                    running: (thumbnailFetcher.fetching ||
                              previewPlayer.status === MediaPlayer.Loading)
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: paddingMedium

                Button {
                    text: qsTr("Download")
                    onClicked: {
                        startDownload(entry.path,
                                      entry.mimeType,
                                      false,
                                      false,
                                      entry.lastModified,
                                      accountWorkers.transferCommandQueue)
                    }
                }
                Button {
                    text: qsTr("Open")
                    onClicked: {
                        startDownload(entry.path,
                                      entry.mimeType,
                                      true,
                                      false,
                                      entry.lastModified,
                                      accountWorkers.transferCommandQueue)
                    }
                }
            }

            // Keep file details centered
            Column {
                width: parent.width
                spacing: paddingTiny

                DetailItem {
                    width: parent.width
                    label: qsTr("File name:")
                    value: entry.name
                }
                DetailItem {
                    width: parent.width
                    label: qsTr("Size:")
                    value: fileDetailsHelper.getHRSize(entry.size)
                }
                DetailItem {
                    width: parent.width
                    label: qsTr("Last modified:")
                    value: Qt.formatDateTime(entry.lastModified, Qt.SystemLocaleShortDate);
                    visible: value.length > 0
                }
                DetailItem {
                    width: parent.width
                    label: qsTr("Type:")
                    value: !entry.isDirectory ? entry.mimeType : qsTr("Directory")
                    visible: value.length > 0
                }
                DetailItem {
                    width: parent.width
                    label: qsTr("Created at:")
                    value: entry.createdAt
                    visible: value.length > 0
                }
                DetailItem {
                    width: parent.width
                    label: qsTr("Entity tag:")
                    value: entry.entityTag
                    visible: value.length > 0
                }
                DetailItem {
                    width: parent.width
                    label: qsTr("File ID:")
                    value: entry.fileId
                    visible: value.length > 0
                }
            }
        }
    }
}
