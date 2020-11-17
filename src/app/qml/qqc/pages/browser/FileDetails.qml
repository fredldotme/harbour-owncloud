import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtMultimedia 5.6
import harbour.owncloud 1.0
import QmlUiSet 1.0
import "qrc:/qml/qqc"
import "qrc:/qml/qqc/controls"
import "qrc:/qml-ui-set"

Page {
    id: pageRoot
    title: entry.name
    focus: true

    readonly property bool destroyable : true
    
    FileDetailsHelper { id: fileDetailsHelper}

    property var entry : null;
    property AccountWorkers accountWorkers : null
    property ThumbnailFetcher thumbnailFetcher : accountWorkers.thumbnailFetcher

    readonly property string imgSrc :
        (!entry.isDirectory && !thumbnailFetcher.fetching &&
         thumbnailFetcher.source !== "") ?
            thumbnailFetcher.source :
            fileDetailsHelper.getIconFromMime(entry.mimeType)

    readonly property bool supportsMediaPreview : !(osIsUbuntuTouch || osIsIOS || osIsMacOs)
    readonly property bool supportsDownloads : !osIsIOS

    readonly property bool isAudio :
        supportsMediaPreview && (!entry.isDirectory &&
         (entry.mimeType.indexOf("audio") === 0))
    readonly property bool isVideo :
        supportsMediaPreview && (!entry.isDirectory &&
         (entry.mimeType.indexOf("video") === 0))
    readonly property bool isAudioVideo : (isAudio || isVideo)

    Component.onCompleted: {
        if (isVideo || entry.isDirectory)
            return;

        thumbnailFetcher.remoteFile = entry.path
        thumbnailFetcher.width = 256
        thumbnailFetcher.height = 256
        thumbnailFetcher.fetch();
        console.log("Fetching thumbnail: " + thumbnailFetcher.remoteFile + " @ " + entry.path)
    }
    Component.onDestruction: {
        if (!isAudioVideo)
            return;

        mediaFeeder.stop()
    }
    
    Flickable {
        contentHeight: mainColumn.height + (paddingLarge*8)
        anchors.fill: parent
        ScrollBar.vertical: ScrollBar {}

        Column {
            id: mainColumn
            anchors.fill: parent
            anchors.topMargin: paddingLarge
            spacing: paddingMedium

            Item {
                // Wider side margins when only showing
                // a default icon from the icon set
                readonly property int margins : (thumbnailFetcher.source === "" && !isAudioVideo)
                                                ? (parent.width/4)
                                                : (parent.width/8)
                id: filePreview
                width: Math.min(256, parent.width / 2)
                height: width
                anchors.horizontalCenter: parent.horizontalCenter

                // Image thumbnail
                Image {
                    id: thumbnail
                    anchors.fill: parent
                    source: (entry.isDirectory
                                ? getFolderIcon("folder")
                                : imgSrc)
                    visible: !isVideo
                }

                // Media player for video and audio preview
                WebDavMediaFeeder {
                    id: mediaFeeder
                    mediaPlayer: previewPlayer
                    settings: accountWorkers.account
                    url: {
                        if (!isAudioVideo)
                            return "";

                        return FilePathUtil.getWebDavFileUrl(entry.path,
                                                             accountWorkers.account)
                    }
                    onUrlChanged: {
                        console.log("URL: " + url)
                    }
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
                    visible: isVideo
                    anchors.fill: parent
                }
                GCButton {
                    anchors.centerIn: parent
                    source:
                        (previewPlayer.playbackState == MediaPlayer.PlayingState) ?
                            "qrc:/icons/theme/actions/32/media-playback-pause.svg" :
                            "qrc:/icons/theme/actions/32/media-playback-start.svg"
                    height: 48
                    width: height
                    visible: isAudioVideo
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
                    visible: running
                    running: (thumbnailFetcher.fetching ||
                              previewPlayer.status === MediaPlayer.Loading)
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: paddingMedium
                visible: !entry.isDirectory

                Button {
                    text: qsTr("Download")
                    visible: supportsDownloads
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
                        const destinationDir = FilePathUtil.destination(accountWorkers.account)
                        const fileName = entry.path.substring(entry.path.lastIndexOf("/") + 1)
                        const localFilePath = destinationDir + entry.path
                        const fileDestination = "file://" + localFilePath
                        const exists = FilePathUtil.fileExists(localFilePath);

                        console.log("fileDestination: " + fileDestination)

                        if (exists) {
                            // Open from cache
                            openFileDestination(fileDestination);
                            return;
                        }

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
            }
        }
    }
}
