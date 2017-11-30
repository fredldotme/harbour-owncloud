function getIconFromMime(mimeType) {
    if(mimeType.indexOf("image") === 0) {
        return "image://theme/icon-l-image";
    } else if(mimeType.indexOf("audio") === 0) {
        return "image://theme/icon-l-music";
    } else if(mimeType.indexOf("video") === 0) {
        return "image://theme/icon-l-video";
    } else {
        return "image://theme/icon-l-document";
    }
}

function getHRSize(bytes) {
    var units = ['bytes', 'kB', 'MB', 'GB', 'TB'];
    var i = 0;

    for(; i < 5 && bytes >= 1024; i++) {
        bytes = bytes / 1024;
    }

    return bytes.toFixed(2) + " " + units[i];
}
